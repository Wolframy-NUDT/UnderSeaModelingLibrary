/**
 * @file sensor_pair.h
 * Container for one sensor pair instance.
 */
#pragma once

#include <usml/sensors/sensor_model.h>
#include <usml/sensors/sensor_listener.h>
#include <usml/sensors/xmitRcvModeType.h>
#include <usml/waveq3d/eigenray_collection.h>
#include <usml/eigenverb/envelope_collection.h>
#include <usml/eigenverb/eigenverb_collection.h>

namespace usml {
namespace sensors{

using namespace waveq3d ;
using namespace eigenverb;

/// @ingroup sensors
/// @{

/**
 * Container for one sensor pair instance.
 * On construction a pointer to the source and receiver sensor are obtained.
 * Inherits the sensor_listener interface so a sensor instance can get
 * access to its complement sensor, and updates the eigenverbs and fathometers.
 */
class USML_DECLSPEC sensor_pair : public sensor_listener
{
public:

	/**
	 * Data type used for reference to receiver_params.
	 */
	typedef shared_ptr<sensor_pair> reference;

    /**
     * Construct from references to source and receiver.
     * The source and receiver will be equal for monostatic sensors.
     *
     * @param	source		Shared pointer to the source for this pair.
     * @param	receiver	Shared pointer to the receiver for this pair.
     */
    sensor_pair(sensor_model::reference source, sensor_model::reference receiver)
        : _source(source),_receiver(receiver) {};

    /**
     * Destructor
     */
    ~sensor_pair() {
    }

    /**
     * Gets a pointer to the source sensor.
     * @return  Pointer to the source sensor
     */
    sensor_model::reference source() const {
        return _source;
    }

     /**
     * Gets a pointer to the receiver sensor.
     * @return  Pointer to the receiver sensor
     */
    sensor_model::reference receiver() const {
        return _receiver;
    }

	/**
	 * Bistatic sensor_pairs are those for which the source and receiver
	 * are different.  Set to false for monostatic sensors.
	 */
	bool multistatic() const {
		return _source != _receiver ;
	}

	/**
	 * Notification that new fathometer data is ready.
	 *
	 * @param  sensor	Sensor that issued the notification.
	 */
	virtual void update_fathometers(sensor_model::reference& sensor) ;

	/**
	 * Notification that new eigenverb data is ready.
	 *
	 * @param	sensor	Sensor that issued the notification.
	 */
	virtual void update_eigenverbs(sensor_model::reference& sensor) ;

	/**
	 * Queries for the sensor pair complements of this sensor.
	 *
	 * @param	sensor	Sensor that issued the notification.
	 */
	virtual sensor_model::reference sensor_complement(sensor_model::reference& sensor) const ;

	/**
     * Gets the shared_ptr to last eigenray_list update for this sensor_pair.
     * @return  eigenray_list shared_ptr
     */
     boost::shared_ptr<eigenray_list> eigenrays() {
         read_lock_guard guard(_eigenrays_mutex);
         return _eigenrays;
     }

private:

    sensor_pair() {};

    /**
     * Sets the shared_ptr to eigenray_list for this sensor_pair.
     * @return  eigenray_list shared_ptr to the eigenray_list
     */
    void eigenrays(eigenray_list* list) {
        write_lock_guard guard(_eigenrays_mutex);
        _eigenrays = boost::shared_ptr<eigenray_list>(list);
    }

    /**
     * Shared pointer to the source sensor.
     * The source and receiver will be equal for monostatic sensors.
     */
    const sensor_model::reference _source;

    /**
     * Share pointer to the receiver sensor.
     * The source and receiver will be equal for monostatic sensors.
     */
    const sensor_model::reference _receiver;

    /**
     * Eigenrays that connect source and receiver locations.
     */
    boost::shared_ptr<eigenray_list> _eigenrays;

	/**
	 * Mutex that locks sensor_pair during eigenray updates.
	 */
	mutable read_write_lock _eigenrays_mutex ;

    /**
     * Interface collisions for wavefront emanating from the source.
     */
    eigenverb_collection::reference _src_eigenverbs;

	/**
	 * Mutex to that locks sensor_pair during source eigenverb updates.
	 */
	mutable read_write_lock _src_eigenverbs_mutex ;

    /**
     * Interface collisions for wavefront emanating from the receiver.
     */
    eigenverb_collection::reference _rcv_eigenverbs;

	/**
	 * Mutex to that locks sensor_pair during receiver eigenverb updates.
	 */
	mutable read_write_lock _rcv_eigenverbs_mutex ;

    /**
     * envelopes - contains the Reverb envelopes
     */
    envelope_collection::reference _envelopes;

	/**
	 * Mutex to that locks sensor_pair during _envelope updates.
	 */
	mutable read_write_lock _envelopes_mutex ;

};

/// @}
} // end of namespace sensors
} // end of namespace usml