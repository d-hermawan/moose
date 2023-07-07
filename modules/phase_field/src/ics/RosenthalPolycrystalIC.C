//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "RosenthalPolycrystalIC.h"
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "libmesh/utility.h"
#include "PolycrystalVoronoi.h"
#include "RosenthalTemperature.h"

registerMooseObject("MooseApp", RosenthalPolycrystalIC);

InputParameters
RosenthalPolycrystalIC::validParams()
{
  InputParameters params = InitialCondition::validParams();
  MooseEnum structure_options("grains voids");
  params.addRequiredParam<MooseEnum>("structure_type",
                                     structure_options,
                                     "Which structure type is being initialized, grains or voids");
  params.addParam<unsigned int>("op_index",
                                    0,
                                    "The index for the current order parameter, "
                                    "not needed if structure_type = voids");
  params.addRequiredParam<UserObjectName>(
      "polycrystal_ic_uo", "UserObject for obtaining the polycrystal grain structure.");
  params.addRequiredParam<UserObjectName>(
      "rosenthal_temperature_uo", "UserObject holding information about the rosenthal temperature profile");
  params.addParam<Real>("invalue", 1, "Scaling factor to apply on the function");
  params.addParam<Real>("outvalue", 0, "Scaling factor to apply on the function");

  return params;
}

RosenthalPolycrystalIC::RosenthalPolycrystalIC(const InputParameters & parameters)
  : InitialCondition(parameters),
    _structure_type(getParam<MooseEnum>("structure_type")),
    _op_index(getParam<unsigned int>("op_index")),
    _poly_ic_uo(getUserObject<PolycrystalVoronoi>("polycrystal_ic_uo")),
    _rosenthal_uo(getUserObject<RosenthalTemperature>("rosenthal_temperature_uo")),
    _invalue(parameters.get<Real>("invalue")),
    _outvalue(parameters.get<Real>("outvalue"))
{
}

Real
RosenthalPolycrystalIC::value(const Point & p)
{
    Real value = 0;
    Real grain_value = _poly_ic_uo.getVariableValue(_op_index, p);
    Real void_value = 0;
    Real temperature = _rosenthal_uo.value(p);
    Real _Tm = _rosenthal_uo.melting_temp();
    void_value = 0.5*std::tanh(0.25*(temperature-_Tm))+0.5;
    
    switch (_structure_type)
    {
    case 0:                 // assigning values for grains (order parameters)
      if (grain_value == 0) // Not in this grain
        value = grain_value;
      else                             // in this grain, but might be in a void
          if (void_value == _outvalue) // Not in a void
        value = grain_value;
      else if (void_value > _outvalue && void_value < _invalue) // On void interface
        value = grain_value * (_invalue - void_value) / (_invalue - _outvalue);
      else if (void_value == _invalue) // In a void, so op = 0
        value = 0.0;
      break;

    case 1: // assigning values for voids (concentration)
      value = void_value;
      break;
  }
    return value;
}