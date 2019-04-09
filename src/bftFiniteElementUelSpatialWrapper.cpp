#include "bftFiniteElementUelSpatialWrapper.h"
#include "Eigen/Sparse"
#include "bftVoigt.h"
#include <iostream>

using namespace Eigen;
BftElementSpatialWrapper::BftElementSpatialWrapper( int                     nDim,
                                            int                     nDimChild,
                                            int                     nNodes,
                                            int                     nRhsChild,
                                            const int               rhsIndicesToBeWrapped[],
                                            int                     nRhsIndicesToBeWrapped,
                                            std::unique_ptr<BftElement> childElement )

    : nDim( nDim ),
      nDimChild( nDimChild ),
      nNodes( nNodes ),
      nRhsChild( nRhsChild ),
      rhsIndicesToBeProjected( rhsIndicesToBeWrapped, nRhsIndicesToBeWrapped ),
      projectedSize( nRhsChild ),
      unprojectedSize( nRhsChild + rhsIndicesToBeProjected.size() * ( nDim - nDimChild ) ),
      childElement( std::move( childElement ) )
{
}

int BftElementSpatialWrapper::getNumberOfRequiredStateVars()
{
    return childElement->getNumberOfRequiredStateVars();
}

std::vector<std::vector<std::string>> BftElementSpatialWrapper::getNodeFields()
{
    return childElement->getNodeFields();
}

int BftElementSpatialWrapper::getNNodes()
{
    return nNodes;
}

int BftElementSpatialWrapper::getNDofPerElement()
{
    return unprojectedSize;
}

std::string BftElementSpatialWrapper::getElementShape()
{
    return childElement->getElementShape();
}

void BftElementSpatialWrapper::assignProperty( const BftMaterialSection& property )
{
    childElement->assignProperty( property );
}

void BftElementSpatialWrapper::assignProperty( const ElementProperties& property )
{
    childElement->assignProperty( property );
}

std::vector<int> BftElementSpatialWrapper::getDofIndicesPermutationPattern()
{
    std::vector<int> permutationPattern;

    auto childPermutationPattern = childElement->getDofIndicesPermutationPattern();

    int i = 0, j = 0, k = 0; // i: projected, j: unprojected, k: indicesToProject
    int indexChild;
    while ( i < projectedSize ) {

        indexChild = childPermutationPattern[i];
        if ( k < rhsIndicesToBeProjected.size() && i == rhsIndicesToBeProjected( k ) ) {
            // copy the Transformation block wise ( if current DOF is projected )
            for ( int l = 0; l < nDim; l++ )
                permutationPattern.push_back( indexChild + j + l );

            i += nDimChild;
            j += ( nDim - nDimChild );
            k += 1;
        }
        else {
            permutationPattern.push_back( indexChild + j );
            i++;
            j++;
        }
    }

    return permutationPattern;
}

void BftElementSpatialWrapper::assignStateVars( double* stateVars, int nStateVars )
{
    childElement->assignStateVars( stateVars, nStateVars );
}

void BftElementSpatialWrapper::initializeYourself( const double* coordinates )
{
    Map<const MatrixXd> unprojectedCoordinates( coordinates, nDim, nNodes );

    if ( nDimChild == 1 ) {
        // take the (normalized) directional vector based on the first 2 nodes (valid for truss2,
        // truss3);
        VectorXd n = unprojectedCoordinates.col( 1 ) - unprojectedCoordinates.col( 0 );
        n /= n.norm();
        T = n.transpose();
    }

    P = MatrixXd::Zero( projectedSize, unprojectedSize );
    // Alternatively: P as a Sparse matrix; currently not necessary...

    int i = 0, j = 0, k = 0; // i: projected, j: unprojected, k: indicesToProject
    while ( i < projectedSize ) {

        if ( k < rhsIndicesToBeProjected.size() && i == rhsIndicesToBeProjected( k ) ) {
            // copy the Transformation block wise ( if current DOF is projected )
            P.block( i, j, nDimChild, nDim ) = T;

            i += nDimChild;
            j += nDim;
            k += 1;
        }
        else {
            P( i, j ) = 1.0;
            i++;
            j++;
        }
    }

    // Projection of node coordinates
    projectedCoordinates = MatrixXd::Zero( nDimChild, nNodes );
    for ( int i = 0; i < nNodes; i++ )
        projectedCoordinates.col( i ) = T * unprojectedCoordinates.col( i );

    childElement->initializeYourself( projectedCoordinates.data() );
}

void BftElementSpatialWrapper::computeYourself( const double* Q,
                                            const double* dQ,
                                            double*       Pe_,
                                            double*       Ke_,
                                            const double* time,
                                            double        dT,
                                            double&       pNewDT )
{
    Map<const VectorXd> Q_Unprojected( Q, unprojectedSize );
    Map<const VectorXd> dQ_Unprojected( dQ, unprojectedSize );

    VectorXd Q_Projected  = P * Q_Unprojected;
    VectorXd dQ_Projected = P * dQ_Unprojected;

    VectorXd Pe_Projected( projectedSize );
    MatrixXd Ke_Projected( projectedSize, projectedSize );

    childElement->computeYourself( Q_Projected.data(),
                                   dQ_Projected.data(),
                                   Pe_Projected.data(),
                                   Ke_Projected.data(),
                                   time,
                                   dT,
                                   pNewDT );

    if ( pNewDT < 1.0 )
        return;

    Map<VectorXd> Pe_Unprojected( Pe_, unprojectedSize );
    Map<MatrixXd> Ke_Unprojected( Ke_, unprojectedSize, unprojectedSize );

    Ke_Unprojected = P.transpose() * Ke_Projected * P;
    Pe_Unprojected = P.transpose() * Pe_Projected;
}

void BftElementSpatialWrapper::setInitialConditions( StateTypes state, const double* values )
{
    childElement->setInitialConditions( state, values );
}

void BftElementSpatialWrapper::computeDistributedLoad( DistributedLoadTypes loadType,
                                                   double*              P_,
                            double* K,
                                                   int                  elementFace,
                                                   const double*        load,
                                                   const double*        QTotal,
                                                   const double*        time,
                                                   double               dT )
{
    VectorXd P_Projected = VectorXd::Zero( projectedSize );
    MatrixXd Ke_Projected( projectedSize, projectedSize );

    childElement->computeDistributedLoad( loadType, P_Projected.data(), Ke_Projected.data(), elementFace, QTotal, load, time, dT );

    Map<VectorXd> P_Unprojected( P_, unprojectedSize );
    P_Unprojected = P.transpose() * P_Projected;

    Map<MatrixXd> Ke_Unprojected( K, unprojectedSize, unprojectedSize );
    Ke_Unprojected = P.transpose() * Ke_Projected * P;
}

void BftElementSpatialWrapper::computeBodyForce( double*       P_,
                            double* K,
                                             const double* load,
                                             const double* QTotal,
                                             const double* time,
                                             double        dT )
{
    VectorXd P_Projected = VectorXd::Zero( projectedSize );
    MatrixXd Ke_Projected( projectedSize, projectedSize );

    childElement->computeBodyForce( P_Projected.data(), Ke_Projected.data(), load, QTotal, time, dT );

    Map<VectorXd> P_Unprojected( P_, unprojectedSize );
    P_Unprojected = P.transpose() * P_Projected;

    Map<MatrixXd> Ke_Unprojected( K, unprojectedSize, unprojectedSize );
    Ke_Unprojected = P.transpose() * Ke_Projected * P;
}

double* BftElementSpatialWrapper::getPermanentResultPointer( const std::string& resultName, int gaussPt, int& resultLength )
{
    if ( resultName == "BftElementSpatialWrapper.T" ) {
        resultLength = T.size();
        return T.data();
    }

    return childElement->getPermanentResultPointer( resultName, gaussPt, resultLength );
}
