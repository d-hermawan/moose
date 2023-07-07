//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "RosenthalPolycrystalICAction.h"
#include "RosenthalPolycrystalIC.h"
#include "Factory.h"
#include "FEProblem.h"
#include "Conversion.h"

registerMooseAction("PhaseFieldApp", RosenthalPolycrystalICAction, "add_ic");

InputParameters
RosenthalPolycrystalICAction::validParams()
{
  InputParameters params = Action::validParams();
  params += RosenthalPolycrystalIC::validParams();
  params.addRequiredParam<unsigned int>("op_num", "Number of order parameters");
  params.addRequiredParam<std::string>("var_name_base", "specifies the base name of the variables");
  params.suppressParameter<VariableName>("variable");
  params.addRequiredParam<UserObjectName>(
      "polycrystal_ic_uo", "UserObject for obtaining the polycrystal grain structure.");
  params.addRequiredParam<UserObjectName>(
      "rosenthal_temperature_uo", "UserObject containing rosenthal temperature parameters and profile");
  params.addParam<std::vector<SubdomainName>>("block",
                                              "Block restriction for the initial condition");

  return params;
}

RosenthalPolycrystalICAction::RosenthalPolycrystalICAction(const InputParameters & params)
  : Action(params),
    _op_num(getParam<unsigned int>("op_num")),
    _var_name_base(getParam<std::string>("var_name_base"))
{
}

void
RosenthalPolycrystalICAction::act()
{
  // Loop through the number of order parameters
  for (unsigned int op = 0; op < _op_num; op++)
  {
    // Set parameters for RosenthalPolycrystalIC
    InputParameters poly_params = _factory.getValidParams("RosenthalPolycrystalIC");
    poly_params.set<unsigned int>("op_index") = op;
    poly_params.set<VariableName>("variable") = _var_name_base + Moose::stringify(op);
    poly_params.set<MooseEnum>("structure_type") = "grains";
    poly_params.set<UserObjectName>("polycrystal_ic_uo") =
        getParam<UserObjectName>("polycrystal_ic_uo");
    poly_params.set<UserObjectName>("rosenthal_temperature_uo") =
        getParam<UserObjectName>("rosenthal_temperature_uo");
    // Add initial condition
    _problem->addInitialCondition(
        "RosenthalPolycrystalIC", name() + "_" + Moose::stringify(op), poly_params);
  }
}
