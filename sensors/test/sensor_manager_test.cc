/**
 * @example sensors/test/sensor_manager_test.cc
 */
#include <usml/sensors/sensor_manager.h>
#include <usml/sensors/sensor_pair_manager.h>
#include <usml/sensors/beam_pattern_map.h>
#include <usml/sensors/source_params_map.h>
#include <usml/sensors/receiver_params_map.h>
#include <boost/test/unit_test.hpp>
#include <iostream>


using namespace boost::unit_test;
using namespace usml::sensors;

BOOST_AUTO_TEST_SUITE(sensor_manager_test)

/**
* @ingroup sensors_test
* @{
*/

BOOST_AUTO_TEST_CASE(pairs_test)
{
    cout << "=== sensor_manager_test: pairs_test ===" << endl;
    
    // insert beam patterns into map

    beam_pattern_map* beam_map = beam_pattern_map::instance();

    beam_pattern_model* beam_patt1 = new beam_pattern_omni();
    beam_pattern_model::id_type id0 = 0;
    beam_pattern_model::id_type id1 = 1;
    beam_patt1->beamID(id1);
    beam_pattern_model::reference beam_heap1(beam_patt1);
    beam_map->insert(id1, beam_heap1);

    std::list<beam_pattern_model::id_type> beamList;
    beamList.push_back(id0);
    beamList.push_back(id1);

    // Source frequencies 6.5K, 7.5K, 8.5K, 9.5K
    seq_linear source_frequencies(6500.0, 1000.0, 4);
    // Receiver frequencies 3.0K, 4.0K, 5.0K, 6.0K, 7.0K, 8.0K, 9.0K, 10.0K
    seq_linear receiver_frequencies(3000, 1000.0, 7);

    // setup SOURCE sensor type  #12 with omni beam pattern
    sensor_params::id_type params1 = 12;
    source_params::reference source1(new source_params(
        params1,	                // paramsID
        vector<double> (1, 123.0),	// source_level
		1.0,					   // pulse_length
		10.0,					   // reverb_duration
        7000.0, 10000.0,            // min, max active freq
        source_frequencies,
        beamList));		           // beam_list
    source_params_map::instance()->insert(source1->paramsID(), source1);

    // setup RECEIVER sensor type #21 
    sensor_params::id_type params2 = 21;
    receiver_params::reference receiver1(new receiver_params(
        params2,	            // paramsID
        5000.0, 9000.0,         // min, max active freq
        receiver_frequencies,
        beamList));
    receiver_params_map::instance()->insert(receiver1->paramsID(), receiver1);

    // Sensor type 33 is for a BOTH mode with src and receiver NOT multistatic
    // setup source side of sensor type #33  BOTH with omni beam pattern
    sensor_params::id_type params3 = 33;
    source_params::reference source3(new source_params(
        params3,	                // paramsID
        vector<double> (1, 130.0),  // source_level
		1.0,					   // pulse_length
		10.0,					   // reverb_duration
        7000.0, 10000.0,            // min, max active freq
        source_frequencies,
        beamList,   		        // beam_list
        false));	                // not multistatic
    source_params_map::instance()->insert(source3->paramsID(), source3);

    // setup receiver side of sensor type #33 BOTH with beam pattern's 0 and 1
    receiver_params::reference receiver3(new receiver_params(
        params3,	            // paramsID
        5000.0, 9000.0,         // min, max active freq
        receiver_frequencies,
        beamList,
        false));	            // not multistatic
    receiver_params_map::instance()->insert(receiver3->paramsID(), receiver3);

    // Vary source and or reciever mutlistatic flag's to test 
    // setup source side of sensor type #44 BOTH with omni beam pattern
    sensor_params::id_type params4 = 44;
    source_params::reference source4(new source_params(
        params4,	                    // paramsID	
        vector<double> (1, 130.0),		// source_level
		1.0,					   // pulse_length
		10.0,					   // reverb_duration
        6000.0, 9000.0,                 // min, max active freq
        source_frequencies,
        beamList,  		                // beamID's
        true));                         // multistatic flag - vary to test different combo's
    source_params_map::instance()->insert(source4->paramsID(), source4);
    // setup receiver side of sensor type #44 with beam pattern's 0 and 1
    receiver_params::reference receiver4(new receiver_params(
        params4,	// paramsID
        5000.0, 9000.0,         // min, max active freq
        receiver_frequencies,
        beamList,
        true));     // multistatic flag - vary to test different combo's
    receiver_params_map::instance()->insert(receiver4->paramsID(), receiver4);


    // Create sensor IDs and param IDs 
    sensor_model::id_type sensors[] = {1, 3, 4, 6, 7, 9};

    sensor_params::id_type sensor_type[] = { 33,   // usml::sensors::BOTH,     // 1 source and receiver NOT multistatic
                                             12,   // usml::sensors::SOURCE,   // 3
                                             21,   // usml::sensors::RECEIVER, // 4
                                             12,   // usml::sensors::SOURCE,   // 6
                                             21,   // usml::sensors::RECEIVER, // 7
                                             44 }; // usml::sensors::BOTH      // 9 Vary source and or reciever mutlistatic flag's to test 

    sensor_manager* manager = sensor_manager::instance();
    for ( unsigned i = 0; i < sizeof(sensors)/sizeof(sensor_model::id_type); ++i ) {
        manager->add_sensor(sensors[i], sensor_type[i]);
    }

    // Attempt to remove a non-existant sensor
    if ( manager->remove_sensor(2) != false ) {
         BOOST_FAIL("pairs_test:: Removed non-existent sensor_model");
    }

    //cout << "=== pairs_test: remove_sensor 1 BOTH ===" << endl;
    if ( manager->remove_sensor(sensors[0]) == false )
    {
         BOOST_FAIL("pairs_test:: Failed to remove sensor_model");
    }

    //cout << "=== pairs_test: remove_sensor 3 SOURCE ===" << endl;
    if ( manager->remove_sensor(sensors[1]) == false )
    {
         BOOST_FAIL("pairs_test:: Failed to remove sensor_model");
    }

    //cout << "=== pairs_test: remove_sensor 4 RECEIVER ===" << endl;
    if ( manager->remove_sensor(sensors[2]) == false )
    {
         BOOST_FAIL("pairs_test:: Failed to remove sensor_model");
    }

    // Expected map contents
     sensor_model::id_type sensors_remaining[] = {6, 7, 9};

    // From
    // xmitRcvModeType sensor_mode[] = { usml::sensors::BOTH,     // 1
    //                                   usml::sensors::SOURCE,   // 3
    //                                   usml::sensors::RECEIVER, // 4
    //                                   usml::sensors::SOURCE,   // 6
    //                                   usml::sensors::RECEIVER, // 7
    //                                   usml::sensors::BOTH }    // 9

     xmitRcvModeType sensor_mode[] = { usml::sensors::SOURCE,   // 6
                                       usml::sensors::RECEIVER, // 7
                                       usml::sensors::BOTH };   // 9

     wposition1 pos(0.0,0.0,-100.0) ;
     vector<double> raxis(3,0) ;
     raxis(0) = 1.0 ;
     orientation orient(0.0,0.0,0.0,raxis) ;
     cout << "Updated sensor 6's depth by 100 m" << endl ;
     sensor_manager::instance()->update_sensor( 6, pos, orient ) ;

     pos.latitude(2.0) ;
     pos.altitude(0.0) ;
     cout << "Updated sensor 7's latitude by 2 deg" << endl ;
     sensor_manager::instance()->update_sensor( 7, pos, orient ) ;

     pos.longitude(2.0) ;
     pos.latitude(0.0) ;
     cout << "Updated sensor 9's longitude by 2 deg" << endl ;
     sensor_manager::instance()->update_sensor( 9, pos, orient ) ;

     orient.pitch(10.0) ;
     pos.longitude(0.0) ;
     cout << "Updated sensor 6's pitch by 10 deg" << endl ;
     sensor_manager::instance()->update_sensor( 6, pos, orient ) ;

     orient.heading(25.0) ;
     orient.pitch(0.0) ;
     cout << "Updated sensor 7's heading by 25 deg" << endl ;
     sensor_manager::instance()->update_sensor( 7, pos, orient ) ;

     orient.roll(15.0) ;
     orient.heading(0.0) ;
     cout << "Updated sensor 9's roll by 15 deg" << endl ;
     sensor_manager::instance()->update_sensor( 9, pos, orient ) ;

     std::pair<sensor_model::id_type, sensor_data> map_data;
     sensor_data_map query;
     for ( int i = 0; i < sizeof(sensors_remaining) / sizeof(sensor_model::id_type); ++i ) {
    	 sensor_data sensor;
    	 sensor._sensorID = sensors_remaining[i];
    	 sensor._mode = sensor_mode[i];
    	 map_data.first = sensor._sensorID;
    	 map_data.second = sensor;
    	 query.insert(map_data);
     }

     // Test find_pair through get_fathometers
     fathometer_collection::fathometer_package fathometers =
        sensor_pair_manager::instance()->get_fathometers(query);

     // Expect zero fathometer for this test!
     BOOST_CHECK_EQUAL(fathometers.size(), 0);
     //cout << "=== pairs_test: fathometers return size " << fathometers.size() << endl;

     // Clean up all singleton to prevent use by other tests!
     source_params_map::reset();
     receiver_params_map::reset();
     beam_pattern_map::reset();
     sensor_pair_manager::reset();
     sensor_manager::reset();

} // end pairs_test

BOOST_AUTO_TEST_SUITE_END()
