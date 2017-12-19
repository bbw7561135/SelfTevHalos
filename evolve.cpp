#include <cmath>
#include <ctime>
#include <iostream>

#include "TGrid2D.h"
#include "units.h"
#include "waves.h"

void Waves::set_dt(const double& dt) {
		this->dt = dt;
		this->dt_half = .5 * dt;
}

void Waves::print_counter2time(const int& max_counter, const int& dump_counter) {
	std::cout << "running for " << max_counter * dt / kyr << " kyr with dt = " << dt / kyr << " kyr\n";
}

int Waves::get_difftime(time_t start) {
	time_t end = time(NULL);
	return difftime(end, start);
}

void Waves::print_status(const size_t& counter, const time_t& start) {
	std::cout << "dump at " << counter * dt / kyr << " kyr after " << get_difftime(start) << " secs\n";
	std::cout << " - f in " << f_cr.min_value() << " ... " << f_cr.max_value() << "\n";
	std::cout << " - dfdz in " << df_dz.min_value() << " ... " << df_dz.max_value() << "\n";
	std::cout << " - D in " << D_zz.min_value() / (cm2 / s) << " ... " << D_zz.max_value() / (cm2 / s) << "\n";
}

double Waves::compute_total_energy_in_fcr() {
	const double R = 2 * pc;
	double value = 0;
	for (size_t iz = 0; iz < z.get_size(); ++iz) {
		double I_p = 0;
		for (size_t ip = 0; ip < p.get_size() - 1; ++ip) {
			I_p += pow3(p.at(ip)) * f_cr.get(ip, iz) * (p.at(ip) * c_light);
		}
		if (par.do_3D())
			value += pow2(z.at(iz)) * I_p;
		else
			value += I_p;
	}
	if (par.do_3D())
		value *= pow2(4. * M_PI) * dz * dlnp;
	else
		value *= 4.0 * pow2(M_PI) * pow2(R) * dz * dlnp;

	return value;
}

double Waves::compute_source_luminosity() {
	const double R = 2 * pc;
	double value = 0;
	for (size_t iz = 0; iz < z.get_size(); ++iz) {
		double I_p = 0;
		for (size_t ip = 0; ip < p.get_size() - 1; ++ip) {
			I_p += pow3(p.at(ip)) * Q_cr.get(ip, iz) * (p.at(ip) * c_light);
		}
		if (par.do_3D())
			value += pow2(z.at(iz)) * I_p;
		else
			value += I_p;
	}
	if (par.do_3D())
			value *= pow2(4. * M_PI) * dz * dlnp;
		else
			value *= 4.0 * pow2(M_PI) * pow2(R) * dz * dlnp;

	return value;
}

void Waves::test_total_energy(const size_t& counter, const double& dt) {
	std::cout << " -- source term: " << compute_source_luminosity() / (erg / s) << "\n";
	double t = (double) counter * dt;
	double t0 = par.source_tdecay();
	double injected = compute_source_luminosity() * (t * t0 / (t + t0));
	double in_crs = compute_total_energy_in_fcr();
	std::cout << " -- injected: " << injected / erg << "\n";
	std::cout << " -- in CRs: " << in_crs / erg << " / " <<  in_crs / injected << "\n";
}

void Waves::test_boundary_conditions() {
	double value = 0;
	for (size_t ip = 0; ip < p.get_size(); ++ip) {
		value += fabs(f_cr.get(ip, z_size - 1));
		value += fabs(f_cr.get(ip, 0));
	}
	for (size_t iz = 0; iz < z.get_size(); ++iz)
		value += fabs(f_cr.get(p_size - 1, iz));
	std::cout << " -- total CRs at border : " << value << "\n";
}

void Waves::evolve(const double& dt, const int& max_counter, const int& dump_counter) {
	set_dt(dt);
	print_counter2time(max_counter, dump_counter);
	time_t start = time(NULL);
	size_t counter = 0;
	while (counter < max_counter) {
		counter++;
		evolve_f_in_z(1, counter * dt);
		//evolve_f_in_p(2, counter * dt);
		if (counter > 10000 && par.do_selfgeneration()) {
			compute_dfdz();
			//evolve_waves_advectice(1);
			evolve_waves(1);
			compute_D_zz();
		}
		if (counter % dump_counter == 0) {
			print_status(counter, start);
			test_total_energy(counter, dt);
			test_boundary_conditions();
			dump(counter * dt);
		} // if
	} // while
	print_status(counter, start);
}
