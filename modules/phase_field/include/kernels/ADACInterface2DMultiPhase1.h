//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADACInterface.h"

/**
 * Compute the Allen-Cahn interface term with the weak form residual
 * \f$ \left(\nabla (L \psi), 1/2 {\partial \kappa} \over {\partial \nabla \eta_{\alpha i}}
 * \sum \{(\nabla \eta_{\beta j})^2 \} \right) \f$
 */
class ADACInterface2DMultiPhase1 : public ADACInterface
{
public:
  static InputParameters validParams();

  ADACInterface2DMultiPhase1(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual();

  /// Interfacial parameter
  const ADMaterialProperty<RealGradient> & _dkappadgrad_etaa;

  /// Order parameters
  const unsigned int _num_etas;
  std::vector<const ADVariableGradient *> _grad_eta;
};