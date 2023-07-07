//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADACInterface2DMultiPhase1.h"

registerMooseObject("PhaseFieldApp", ADACInterface2DMultiPhase1);

InputParameters
ADACInterface2DMultiPhase1::validParams()
{
  InputParameters params = ADACInterface::validParams();
  params.addClassDescription(
      "Gradient energy Allen-Cahn Kernel where the derivative of interface parameter kappa "
      "wrt the gradient of order parameter is considered.");
  params.addParam<MaterialPropertyName>("dkappadgrad_etaa_name",
                                        "dkappadgrad_etaa",
                                        "The derivative of the kappa with respect to grad_etaa");
  params.addRequiredCoupledVar(
      "etas", "All other coupled order parameters eta_i of the multiphase problem");
  return params;
}

ADACInterface2DMultiPhase1::ADACInterface2DMultiPhase1(const InputParameters & parameters)
  : ADACInterface(parameters),
    _dkappadgrad_etaa(getADMaterialProperty<RealGradient>("dkappadgrad_etaa_name")),
    _num_etas(coupledComponents("etas")),
    _grad_eta(_num_etas)
{
  for (unsigned int i = 0; i < _num_etas; ++i)
    _grad_eta[i] = &adCoupledGradient("etas", i);
}

ADReal
ADACInterface2DMultiPhase1::computeQpResidual()
{
  // nabla_Lpsi is the product rule gradient \f$ \nabla (L\psi) \f$
  ADRealVectorValue nabla_Lpsi = _prop_L[_qp] * _grad_test[_i][_qp];

  if (_variable_L)
  {
    ADRealVectorValue grad_L = _grad_u[_qp] * (*_dLdop)[_qp];
    for (unsigned int i = 0; i < _nvar; ++i)
      grad_L += (*_gradarg[i])[_qp] * (*_dLdarg[i])[_qp];

    nabla_Lpsi += grad_L * _test[_i][_qp];
  }

  ADReal SumSquareGradOp = _grad_u[_qp] * _grad_u[_qp];
  for (unsigned int i = 0; i < _num_etas; ++i)
    SumSquareGradOp += (*_grad_eta[i])[_qp] * (*_grad_eta[i])[_qp];

  return 0.5 * nabla_Lpsi * _dkappadgrad_etaa[_qp] * SumSquareGradOp;
}