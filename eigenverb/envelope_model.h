/**
 * @file envelope_model.h
 * Computes the reverberation envelope time series for a single combination of
 * receiver azimuth, source beam number, receiver beam number.
 */
#pragma once

#include <usml/types/seq_vector.h>
#include <usml/eigenverb/eigenverb.h>

namespace usml {
namespace eigenverb {

using namespace usml::types;

class envelope_collection ; // forward declaration

/**
 * Computes the reverberation envelope time series for a single combination of
 * receiver azimuth, source beam number, receiver beam number. The envelope is
 * stored as a matrix that represents the results a function of the
 * sensor_pair's transmit frequency (rows) and two-way travel time (columns).
 *
 * There are no public methods in this class.  It just acts as a set of service
 * routines for the envelope_collection class.
 *
 * @xref S. Reilly, D. Thibaudeau, T. Burns, "Fast computation of
 * reverberation using Gaussian beam reflections," Report to NAWCTSD,
 * October 20, 2014.
 */
class USML_DECLSPEC envelope_model {

friend class envelope_collection ;

private:

	/**
	 * Reserve the memory used to store the results of this calculation.
	 *
	 * @param transmit_freq	Frequencies at which the source and receiver
	 * 						eigenverbs are computed (Hz).
	 * @param travel_time	Times at which the sensor_pair's
	 * 						reverberation envelopes are computed (Hz).
	 * @param pulse_length	Duration of the transmitted pulse (sec).
	 * 						Defines the temporal resolution of the envelope.
	 * @param threshold		Minimum intensity level for valid reverberation
	 * 						contributions (linear units).
	 */
	envelope_model(
		const seq_vector* transmit_freq,
		const seq_vector* travel_time,
		double pulse_length, double threshold
	) ;

	/**
	 * Default constructor.
	 */
	~envelope_model() {}

	/**
	 * Computes the intensity for a single combination of source and receiver
	 * eigenverbs.  Assumes that the source and receiver eigenverbs
	 * have been interpolated onto the sensor_pair's frequency domain before
	 * this routine is called. It also assumes that the calling routine
	 * has computed the scattering coefficient; which saves this
	 * class from having to know anything about the ocean.
	 *
	 * @param src_verb		Eigenverb contribution from the source.
	 * @param rcv_verb		Eigenverb contribution from the receiver.
	 * @param scatter		Scattering strength coefficient for this
	 * 						combination of eigenverbs (ratio).
	 * @param pulse_length	Duration of the transmit pulse (sec).
	 * @return				False if reverbereation energy below threshold.
	 */
	bool compute_intensity( const vector<double>& scatter,
			const eigenverb& src_verb, const eigenverb& rcv_verb ) ;

	/**
	 * Reverberation intensity at each point the time series.
	 * Each row represents a specific transmit frequency.
	 * Each column represents a specific travel time.
	 */
	matrix< double >& intensity() {
		return _intensity ;
	}

private:

	/**
	 * Compute the total energy and duration of the overlap between
	 * two eigenverbs. Implements the analytic solution for energy of
	 * the bistatic reverberation contribution from eqn. (28) ans (29)
	 * in the paper.  Computes the duration from eqn. (45) and (33).
	 *
	 * @param scatter		Scattering strength coefficient for this
	 * 						combination of eigenverbs,
	 * 						as a function of transmit frequency (ratio).
	 * @param src_verb		Eigenverb contribution from the source.
	 * @param rcv_verb		Eigenverb contribution from the receiver.
	 * @return				False if energy below threshold.
	 */
	bool compute_overlap( const vector<double>& scatter,
			const eigenverb& src_verb, const eigenverb& rcv_verb ) ;

	/**
	 * Computes Gaussian time series contribution given delay, duration, and
	 * total energy.  Implements equation (6) from the paper.  Replaces the
	 * values previously held by the _intensity member variable.
	 *
	 * @param src_verb		Eigenverb contribution from the source.
	 * @param rcv_verb		Eigenverb contribution from the receiver.
	 */
	void compute_time_series(
			const eigenverb& src_verb, const eigenverb& rcv_verb ) ;

	/**
	 * Frequencies at which the source and receiver eigenverbs are computed (Hz).
	 */
	const seq_vector* _transmit_freq ;

	/**
	 * Times at which the sensor_pair's reverberation envelopes
	 * are computed (sec).
	 */
	const vector<double> _travel_time ;

	/**
	 * Duration of the transmitted pulse (sec).
	 * Defines the temporaal resolution of the envelope.
	 */
	const double _pulse_length ;

	/**
	 * Minimum energy level for valid reverberation contributions
	 * (linear units).  Note that this is converted from intensity units
	 * in the constructor.
	 */
	const double _threshold ;

	/**
	 * Workspace for storing total energy of eigenverb overlap (linear units).
	 * Making is a member variable prevents us from having to
	 * rebuilt it for each calculation.
	 */
	vector<double> _energy ;

	/**
	 * Workspace for storing duration result of eigenverb overlap (sec).
	 * Making is a member variable prevents us from having to
	 * rebuilt it for each calculation.
	 */
	vector<double> _duration ;

	/**
	 * Reverberation intensity at each point the time series.
	 * Each row represents a specific transmit frequency.
	 * Each column represents a specific travel time.
	 */
	matrix< double > _intensity;
};

}   // end of namespace eigenverb
}   // end of namespace usml
