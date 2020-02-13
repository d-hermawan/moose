[GlobalParams]
  order = FIRST
  family = LAGRANGE
[]

[XFEM]
  qrule = volfrac
  output_cut_plane = true
[]

[UserObjects]
  [./line_seg_cut_uo1]
    type = LineSegmentCutUserObject
    cut_data = '-0.0 0.5 0.5 0.5'
    time_start_cut = 0.0
    time_end_cut = 0.0
  [../]
  [./line_seg_cut_uo2]
    type = LineSegmentCutUserObject
    cut_data = '0.5 0.5 1.0 0.7'
    time_start_cut = 0.0
    time_end_cut = 0.0
  [../]
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
  xmin = 0.0
  xmax = 1.0
  ymin = 0.0
  ymax = 1.0
  elem_type = QUAD4
  displacements = 'disp_x disp_y'
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
[]

[SolidMechanics]
  [./solid]
    disp_x = disp_x
    disp_y = disp_y
    use_displaced_mesh = false
  [../]
[]

[BCs]
  [./top_x]
    type = DirichletBC
    boundary = 2
    variable = disp_x
    value = 0.0
  [../]
  [./top_y]
    type = DirichletBC
    boundary = 2
    variable = disp_y
    value = 0.1
  [../]
  [./bottom_y]
    type = DirichletBC
    boundary = 0
    variable = disp_y
    value = -0.1
  [../]
  [./bottom_x]
    type = DirichletBC
    boundary = 0
    variable = disp_x
    value = 0.0
  [../]
[]

[Materials]
  [./linelast]
    type = LinearIsotropicMaterial
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    poissons_ratio = 0.3
    youngs_modulus = 1e6
  [../]
[]

[Executioner]
  type = Transient

  solve_type = 'PJFNK'
  petsc_options_iname = '-ksp_gmres_restart -pc_type -pc_hypre_type -pc_hypre_boomeramg_max_iter'
  petsc_options_value = '201                hypre    boomeramg      8'

  line_search = 'none'

  [./Predictor]
    type = SimplePredictor
    scale = 1.0
  [../]

# controls for linear iterations
  l_max_its = 100
  l_tol = 1e-2

# controls for nonlinear iterations
  nl_max_its = 15
  nl_rel_tol = 1e-16
  nl_abs_tol = 1e-10

# time control
  start_time = 0.0
  dt = 1.0
  end_time = 1.0
[]

[Outputs]
  exodus = true
  [./console]
    type = Console
    output_linear = true
  [../]
[]