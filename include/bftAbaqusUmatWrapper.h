#pragma once 
#include "bftTypedefs.h"

namespace bft{

	void simpleUmat(Ref<MatrixXd> abqC,
                    Ref<VectorXd> abqStress,
                    Ref<VectorXd> abqStateVars,
                    const  Ref<const VectorXd>& abqStrain,
                    const  Ref<const VectorXd>& abqDStrain,
                    const  Ref<const Eigen::VectorXd>& matProps,
                    const int nProps,
                    double& pNewdT,
                    const double charElemlen,
                    const double time[2],
                    const double dT,
                    const int nDirect,
                    const int nShear,
                    const int nTensor,
                    const int noEl,
                    const int materialID,
                    pUmatType umatPointer);

	void simpleUmatNonLocal(Ref<MatrixXd>                       dStressdStrain,
                            Ref<VectorXd>                       stress,
                            double&                             intParamLocal,
                            Ref<Vector6>                        dStressDIntParamNonlocal,
                            Ref<Vector6>                        dIntParamLocalDStrain,
                            double&                             nonLocalRadius,
                            Ref<VectorXd>                       stateVars,
                            const  Ref<const VectorXd>&         strainOld,
                            const  Ref<const VectorXd>&         dStrain,
                            double                              intParamNonLocalOld,
                            double                              dIntParamNonLocal,
                            const  Ref<const VectorXd>&         matProps,
                            double&                             pNewdT,
                            double                              time[2],
                            double                              dT,
                            int                                 nDirect,
                            int                                 nShear,
                            int                                 nTensor,
                            int                                 noEl,
                            int                                 noGaussPt,
                            pUmatType                           umatPointer);

    void umatPlaneStress(	Ref<Matrix6> dStressdStrain,
	                        Ref<Vector6> stress,
                            Ref<VectorXd> stateVars,
	                        const Ref<const Vector6>& strain,
	                        Ref<Vector6> dStrain,
                            const Ref<const VectorXd>& matProps,
	                        const int nProps,
                            double& pNewdT,
                            const double charElemlen,
                            const double time[2],
                            const double dT,
	                        const int noEl,
                            const int materialID,
                            pUmatType umatPointer);

	void simpleUmatPlaneStressNonLocal(
                            Ref<MatrixXd>                       dStressdStrain,
                            Ref<VectorXd>                       stress,
                            double&                             intParamLocal,
                            Ref<Vector6>                        dStressDIntParamNonlocal,
                            Ref<Vector6>                        dIntParamLocalDStrain,
                            double&                             nonLocalRadius,
                            Ref<VectorXd>                       stateVars,
                            const  Ref<const VectorXd>&         strainOld,
                            Ref<VectorXd>                       dStrain,
                            double                              intParamNonLocalOld,
                            double                              dIntParamNonLocal,
                            const  Ref<const VectorXd>&         matProps,
                            double&                             pNewdT,
                            double                              time[2],
                            double                              dT,
                            int                                 nDirect,
                            int                                 nShear,
                            int                                 nTensor,
                            int                                 noEl,
                            int                                 noGaussPt, 
                            pUmatType                           umatPointer);

    void umatPlaneStressBisectionMethod (Ref<Matrix6> Cep,
	                        Ref<Vector6> stress,
                            Ref<VectorXd> stateVars,
	                        const Ref<const Vector6>& strain,
	                        Ref<Vector6> dStrain,
                            const Ref<const VectorXd>& matProps,
	                        const int nProps,
                            double& pNewdT,
                            const double charElemlen,
                            const double time[2],
                            const double dT,
	                        const int noEl,
                            const int materialID,
                            pUmatType umatPointer);
}
