//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "InitialCondition.h"
#include "PolycrystalICTools.h"

// Forward Declarationsc
class PolycrystalVoronoi;
class InputParameters;
class RosenthalTemperature;

InputParameters validParams();

/**
 * Defines a boundary condition that forces the value to be a user specified
 * function at the boundary.
 */
class RosenthalPolycrystalIC : public InitialCondition
{
public:
  static InputParameters validParams();

  RosenthalPolycrystalIC(const InputParameters & parameters);

protected:
  const MooseEnum _structure_type;
  const unsigned int _op_index;
  const PolycrystalVoronoi & _poly_ic_uo;
  const RosenthalTemperature & _rosenthal_uo;
  /**
   * The value of the variable at a point.
   */
  virtual Real value(const Point & p) override;

  const Real _invalue;
  const Real _outvalue;

};
