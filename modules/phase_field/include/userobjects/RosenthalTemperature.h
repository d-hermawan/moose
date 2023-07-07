#pragma once

#include "libmesh/mesh_tools.h"
#include "GeneralUserObject.h"

class RosenthalTemperature : public GeneralUserObject
{
public:
    static InputParameters validParams();

    RosenthalTemperature(const InputParameters & parameters);

    virtual void initialize() final {}
    virtual void execute() final {}
    virtual void finalize() final {}

    virtual Real thermal_conductivity() const;
    virtual Real density() const;
    virtual Real specific_heat() const;
    virtual Real power() const;
    virtual Real velocity() const;
    virtual Real initial_x() const;
    virtual Real initial_y() const;
    virtual Real melting_temp() const;
    virtual Real ambient_temp() const;
    virtual Real maximum_temp() const;


    virtual Real value(const Point & p) const;

    virtual Real spatialValue(const Point & p) const;

protected:
    Real _kappa;
    Real _rho;
    Real _cp;
    Real _Q;
    Real _Vp;
    Real _x0;
    Real _y0;
    Real _Tm;
    Real _To;
    Real _Tmax;
};