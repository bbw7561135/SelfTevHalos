// Copyright (c) 2018 carmeloevoli distributed under the MIT License
#ifndef WAVES_H_
#define WAVES_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "TAxis.h"
#include "TGrid2D.h"
#include "analytical_solution.h"
#include "params.h"
#include "tridiag.h"
#include "units.h"
#include "utilities.h"

namespace CRWAVES {

class Waves {
 public:
  Waves(const Params& params);
  virtual ~Waves();

  // waves.cpp
  void build_p_axis(const double& pc_min, const double& pc_max, const size_t& pc_size);
  void build_z_axis(const double& halo_size, const size_t& k_size);
  double compute_constant_CR_source_term_1D();
  double compute_constant_CR_source_term_3D();
  double compute_constant_CR_source_term();

  void build_CR_source_term();
  void build_W_ISM();
  void build_W_sg();
  void build_f_cr();
  void build_D_zz();
  void build_v_A();
  void build_energy_losses();
  void build_analytical_solution();

  // dump.cpp
  std::string generate_output_filename(const std::string& s, const double& t);
  void dump(const double& t);
  void dump_analytical_test(const double& t);

  // diffusion.cpp
  void compute_D_zz();
  void compute_dfdz();
  void compute_dfdz_1D();
  void compute_dfdz_3D();

  // evolutors.cpp
  void evolve_f_in_z(const size_t& number_of_operators, const double& t_now);
  void evolve_f_in_z_1D(const size_t& number_of_operators, const double& t_now);
  void evolve_f_in_z_3D(const size_t& number_of_operators, const double& t_now);
  void evolve_f_in_p(const size_t& number_of_operators, const double& t_now);
  void evolve_waves(const size_t& number_of_operators);

  // evolve.cpp
  void set_dt(const double& dt);
  void print_counter2time(const int& max_counter, const int& dump_counter);
  int get_difftime(time_t start);
  void print_status(const size_t& counter, const time_t& start);
  double compute_total_energy_in_fcr();
  double compute_source_luminosity();
  void test_total_energy(const size_t& counter, const double& dt);
  void test_boundary_conditions();
  void test_courant_conditions();
  void evolve(const double& dt, const int& max_counter, const int& dump_counter);

 private:
  Params par;
  double dt = 0;
  double dt_half = 0;
  double dz = 0;
  double dlnp = 0;
  size_t z_size = 0;
  size_t p_size = 0;

  double magnetic_energy_density = 0;
  double vA_infty = 0;
  double k0 = 0;

  double factor_damping = 0;
  double factor_growth = 0;

  AnalyticSolution solution;
  TAxis<double> p;
  TAxis<double> z;
  TGrid2D<double> Q_cr;
  TGrid2D<double> W_ISM;
  TGrid2D<double> W_sg;
  TGrid2D<double> f_cr;
  TGrid2D<double> df_dz;
  TGrid2D<double> D_zz;
  TGrid2D<double> v_A;
  TGrid2D<double> dp_dt;
};

}  // namespace CRWAVES

#endif /* WAVES_H_ */
