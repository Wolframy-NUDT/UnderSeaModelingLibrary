/**
 * @file envelope_collection.h
 * Computes the reverberation envelope time series for all combinations of
 * receiver azimuth, source beam number, receiver beam number.
 */
#pragma once

#include <usml/eigenverb/envelope_model.h>
#include <usml/types/seq_linear.h>

namespace usml {
namespace eigenverb {

using namespace usml::types;

/**
 * Computes and stores the reverberation envelope time series for all
 * combinations of receiver azimuth, source beam number, receiver beam number.
 * Relies on envelope_model to calculate the actual time series for each
 * transmit frequency.  Each envelope is stored as a matrix that represents
 * the results a function of the sensor_pair's transmit frequency (rows)
 * and two-way travel time (columns).
 */
class USML_DECLSPEC envelope_collection {

public:

	// shared pointer
	typedef boost::shared_ptr<envelope_collection> reference;

	/**
	 * Reserve memory in which to store results as a series of
	 * nested dynamic arrays.
	 *
	 * @param transmit_freq	Frequencies at which the source and receiver
	 * 						eigenverbs are computed (Hz).
	 * @param num_times		Number of times in the reverberation time series.
	 * @param time_step		Sampling period of the reverberation time series.
	 * @param pulse_length	Duration of the transmitted pulse (sec).
	 * 						Defines the temporal resolution of the envelope.
	 * @param threshold		Minimum intensity level for valid reverberation
	 * 						contributions (linear units).
	 * @param num_azimuths	Number of receiver azimuths in result.
	 * @param num_src_beams	Number of source beams in result.
	 * @param num_rcv_beams Number of receiver beams in result.
	 */
	envelope_collection(
		const seq_vector* transmit_freq,
		size_t num_times,
		double time_step,
		double pulse_length,
		double threshold,
		size_t num_azimuths,
		size_t num_src_beams,
		size_t num_rcv_beams ) ;

	/**
	 * Delete dynamic memory in each of the nested dynamic arrays.
	 */
	~envelope_collection();

	/**
	 * Frequencies at which the source and receiver eigenverbs are computed (Hz).
	 */
	const seq_vector* transmit_freq() const {
		return _transmit_freq ;
	}

	/**
	 * Times at which the sensor_pair's reverberation envelopes
	 * are computed (sec).
	 */
	const seq_vector* travel_time() const {
		return &_travel_time;
	}

	/**
	 * Duration of the transmitted pulse (sec).
	 * Defines the temporaal resolution of the envelope.
	 */
	double pulse_length() const {
		return _pulse_length;
	}

	/**
	 * Minimum energy level for valid reverberation contributions
	 * (linear units).
	 */
	double threshold() const {
		return _threshold ;
	}

	/** Number of receiver azimuths in result. */
	size_t num_azimuths() const {
		return _num_azimuths;
	}

	/** Number of receiver azimuths in result. */
	size_t num_src_beams() const {
		return _num_src_beams;
	}

	/** Number of receiver azimuths in result. */
	size_t num_rcv_beams() const {
		return _num_rcv_beams;
	}

	/**
	 * Intensity time series for one combination of parameters.
	 *
	 * @param azimuth	Receiver azimuth number.
	 * @param src_beam	Source beam number.
	 * @param rcv_beam	Receiver beam number
	 * @return			Reverberation intensity at each point the time series.
	 * 					Each row represents a specific transmit frequency.
	 * 					Each column represents a specific travel time.
	 */
	const matrix< double >& envelope(
		size_t azimuth, size_t src_beam, size_t rcv_beam ) const
	{
		return *_envelopes[azimuth][src_beam][rcv_beam];
	}

	/**
	 * Adds the intensity contribution for a single combination of source
	 * and receiver eigenverbs.  Loops over source and receiver beams to
	 * apply beam pattern to each contribution.
	 *
	 * Assumes that the source and receiver eigenverbs have been interpolated
	 * onto the sensor_pair's frequency domain before this routine is called.
	 * It also assumes that the calling routine has computed the scattering
	 * coefficient and beam levels for this combination of eigenverbs,
	 *
	 * @param azimuth	Receiver azimuth number.
	 * @param scatter	Scattering strength at each transmit frequency (ratio).
	 * @param src_beam	Source beam level at each transmit frequency (ratio).
	 * 					Each row represents a specific transmit frequency.
	 * 					Each column represents a beam number.
	 * @param rcv_beam	Receiver beam level at each transmit frequency (ratio).
	 * 					Each row represents a specific transmit frequency.
	 * 					Each column represents a beam number.
	 * @param src_verb	Eigenverb contribution from the source.
	 * @param rcv_verb	Eigenverb contribution from the receiver.
	 */
	void add_contribution( size_t azimuth, const vector<double>& scatter,
		const matrix<double>& src_beam, const matrix<double>& rcv_beam,
		const eigenverb& src_verb, const eigenverb& rcv_verb ) ;

	/**
	 * Writes the envelope data to disk
	 */
	void write_netcdf(const char* filename) const ;

private:

	/**
	 * Frequencies at which the source and receiver eigenverbs are computed (Hz).
	 */
	const seq_vector* _transmit_freq ;

	/**
	 * Times at which the sensor_pair's reverberation envelopes
	 * are computed (sec).
	 */
	seq_linear _travel_time ;

	/**
	 * Duration of the transmitted pulse (sec).
	 * Defines the temporaal resolution of the envelope.
	 */
	double _pulse_length ;

	/**
	 * Minimum energy level for valid reverberation contributions
	 * (linear units).
	 */
	double _threshold ;

	/** Number of receiver azimuths in result. */
	size_t _num_azimuths;

	/** Number of source beams in result. */
	size_t _num_src_beams;

	/** Number of receiver beams in result. */
	size_t _num_rcv_beams;

	/**
	 * Engine for computing Gaussian envelope contributions.
	 */
	envelope_model _envelope_model ;

	/**
	 * Reverberation envelopes for each combination of parameters.
	 * The order of indices is azimuth number, source beam number,
	 * and then receiver beam number.  Each envelope is stored as a
	 * matrix that represents the results a function of the sensor_pair's
	 * transmit frequency (rows) and two-way travel time (columns).
	 */
	matrix< double >**** _envelopes;
};

}   // end of namespace eigenverb
}   // end of namespace usml
