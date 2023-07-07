[Mesh]
    type = GeneratedMesh
    dim = 2
    xmin = 0
    xmax = 10
    ymin = -5
    ymax = 5
    nx = 10
    ny = 10
    elem_type = QUAD4
[]
[GlobalParams]
    int_width = 0.5
    op_num = 1
    var_name_base = gr
[]
[Variables]
    [./PolycrystalVariables]
    [../]
    [./w]
    [../]
    [./liquid]
    [../]
[]
[UserObjects]
    [./voronoi]
        type = PolycrystalVoronoi
        grain_num = 1
        rand_seed = 227
        coloring_algorithm = bt # We must use bt to force the UserObject to assign one grain to each op
    [../]
    [rosenthal]
        type = RosenthalTemperature
        thermal_conductivity = 3e-5
        specific_heat = 650
        density = 8e-9
        melting_temp = 1660
        ambient_temp = 300
        maximum_temp = 1670
        velocity = 1
        x0 = 8
        y0 = 0
        power = 0.5
    []
[]
[ICs]
    [./bubble_IC]
        variable = liquid
        invalue = 1.0
        outvalue = 0.0
        type = RosenthalPolycrystalIC
        structure_type = voids
        rosenthal_temperature_uo = rosenthal
        polycrystal_ic_uo = voronoi
    [../]
    [./PolycrystalICs]
        [./RosenthalPolycrystalIC]
            structure_type = grains
            polycrystal_ic_uo = voronoi
            rosenthal_temperature_uo = rosenthal
        [../]
    [../]
    [./bnds]
        type = BndsCalcIC # IC is created for activating the initial adaptivity
        variable = bnds
    [../]
[]
[AuxVariables]
    [./bnds]
        order = FIRST
        family = LAGRANGE
    [../]
    [./T]
        order = FIRST
        family = LAGRANGE
    [../]
    [./dTdt]
        order = CONSTANT
        family = MONOMIAL
    [../]
[]
[AuxKernels]
    [./bnds_aux]
        type = BndsCalcAux
        variable = bnds
        execute_on = TIMESTEP_BEGIN
        v = 'liquid gr0'
    [../]
    [./temp_src]
        type = SpatialUserObjectAux
        variable = T
        user_object = rosenthal
    [../]
    [./temp_deriv]
        type = TimeDerivativeAux
        variable = dTdt
        functor = T
    [../]
[]
[Materials]
    [./hl]
        type = SwitchingFunctionMultiPhaseMaterial
        all_etas = 'liquid gr0'
        phase_etas = liquid
        h_name = hl
        outputs = 'exodus'
    [../]
    [./hs]
        type = SwitchingFunctionMultiPhaseMaterial
        all_etas = 'liquid gr0'
        phase_etas = 'gr0'
        h_name = hs
        outputs = 'exodus'
    [../]
    [./omegal]
        type = DerivativeParsedMaterial
        coupled_variables = 'w T'
        property_name = omegal
        material_property_names = 'Vm kl cleq S Tm'
        expression = '-0.5*w^2/Vm^2/kl-w/Vm*cleq - S*(T-Tm)/Tm'
    [../]
    [./omegas]
        type = DerivativeParsedMaterial
        coupled_variables = 'w T'
        property_name = omegas
        material_property_names = 'Vm ks cseq'
        expression = '-0.5*w^2/Vm^2/ks-w/Vm*cseq'
    [../]
    [./rhol]
        type = DerivativeParsedMaterial
        coupled_variables = 'w'
        property_name = rhol
        material_property_names = 'Vm kl cleq'
        expression = 'w/Vm^2/kl + cleq/Vm'
    [../]
    [./rhos]
        type = DerivativeParsedMaterial
        coupled_variables = 'w'
        property_name = rhos
        material_property_names = 'Vm ks cseq'
        expression = 'w/Vm^2/ks + cseq/Vm'
    [../]
    [./const]
        type = GenericConstantMaterial
        prop_names =  'D    chi  Vm   ks    cseq kl    cleq  gab mu   S   Tm   kappa Ll  Ls'
        prop_values = '1.0  0.1  1.0  10.0  0.1  10.0  0.9   1.5 10.0 3e3 1660 0.5   3.0 0.03'
    [../]
    [./Mobility]
        type = ParsedMaterial
        property_name = Dchi
        material_property_names = 'D chi'
        expression = 'D*chi'
    [../]
    [mobility]
        type = ParsedMaterial
        property_name = L
        material_property_names = 'Ls Ll gbloc1 temp_change'
        coupled_variables = 'liquid gr0'
        expression = 'Ls*(1- (0.5*tanh(20*((1-(gr0))-0.1)) + 0.5)) + Ll*(0.5*tanh(20*((1-(gr0))-0.1)) + 0.5)*temp_change'
        outputs = 'exodus'
    []
    [gbloc1]
        type = ParsedMaterial
        property_name = gbloc1
        coupled_variables = 'gr0'
        expression = '(1-(gr0))^3 * (10 - 15*(1-(gr0)) + 6*(1-(gr0))^2)'
        outputs = 'exodus'
    []
    [tempchange]
        type = ParsedMaterial
        property_name = temp_change
        coupled_variables = 'dTdt'
        expression = 'if (dTdt > 0, 2, 1)'
        outputs = 'exodus'
    []
[]
[Modules]
    [./PhaseField]
        [./GrandPotential]
            free_energies_gr = 'omegal omegas'
            free_energies_op = 'omegal omegas'
            free_energies_w = 'rhol rhos'
            switching_function_names = 'hl hs'
            additional_ops = 'liquid'
            chemical_potentials = w
            gamma_gr = gab
            gamma_grxop = gab
            gamma_op = gab
            kappa_gr = kappa
            kappa_op = kappa
            susceptibilities = chi
            mobilities = Dchi
            mobility_name_gr = L
            mobility_name_op = Ll
        [../]
    [../]
[]
[Preconditioning]
    [./SMP]
        type = SMP
        full = true
    [../]
[]
[Executioner]
    type = Transient
    scheme = bdf2
    solve_type = PJFNK
    petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart -pc_factor_shift_type'
    petsc_options_value = 'hypre    boomeramg      31                  nonzero'
    l_tol = 1.0e-7
    l_max_its = 20
    nl_max_its = 20
    nl_rel_tol = 1.0e-6
    nl_abs_tol = 1e-6
    end_time = 2.0
    [./TimeStepper]
        type = IterationAdaptiveDT
        dt = 1e-4
        cutback_factor = 0.5
        growth_factor = 2.0
        optimal_iterations = 8 
        iteration_window = 2
    [../]
[]
[Adaptivity]
  initial_steps = 4
  max_h_level = 4
  marker = err_bnds
 [./Markers]
    [./err_bnds]
      type = ErrorFractionMarker
      coarsen = 0.3
      refine = 0.9
      indicator = ind_bnds
    [../]
  [../]
  [./Indicators]
     [./ind_bnds]
       type = GradientJumpIndicator
       variable = bnds
    [../]
  [../]
[]
[Outputs]
    exodus=true
[]

