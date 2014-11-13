/**
 * @file boundary_model.h
 * Generic interface for the ocean's surface or bottom.
 */

#ifndef USML_OCEAN_BOUNDARY_MODEL_H
#define USML_OCEAN_BOUNDARY_MODEL_H

#include <usml/ocean/reflect_loss_constant.h>
#include <usml/ocean/scattering_model.h>

namespace usml {
namespace ocean {

using boost::numeric::ublas::vector;

/// @ingroup boundaries
/// @{

/**
 * A "boundary model" computes the environmental parameters of
 * the ocean's surface or bottom.  The modeled properties include
 * the depth and reflection properties of the interface.
 * This class implements a reflection loss model through delegation.
 * The delegated model is defined separately and added to its host
 * during/after construction.  The host is defined as a reflect_loss_model
 * subclass so that it's children can share the reflection loss model
 * through this delegation.
 *
 * This implementation defines the unit normal using cartesian coordinates
 * in the \f$(\rho,\theta,\phi)\f$ directions relative to its location.
 * Given this definition, the normal can be computed from the depth
 * derivatives or slope angles using:
 * \f[
 *          s_\theta = tan(\sigma_\theta) = \frac{1}{\rho} \frac{ \partial h }{ \partial \theta }
 * \f]\f[
 *          s_\phi = tan(\sigma_\phi) = \frac{1}{\rho sin(\theta)} \frac{ \partial h }{ \partial \phi}
 * \f]\f[
 *          n_\theta = - sin(\sigma_\theta) = - \frac{ s_\theta }{ \sqrt{ 1 + s_\theta^2 } }
 * \f]\f[
 *          n_\phi = - sin(\sigma_\phi) = - \frac{ s_\phi }{ \sqrt{ 1 + s_\phi^2 } }
 * \f]\f[
 *          n_\rho = \sqrt{ 1 - ( n_\theta^2 + n_\phi^2 ) }
 * \f]
 * where:
 *   - \f$ (\rho,\theta,\phi) \f$ = location at which normal is computed,
 *   - \f$ ( \frac{\partial h}{\partial \theta}, \frac{\partial h}{\partial \phi} ) \f$
 *     = depth derivative in the \f$(\rho,\theta)\f$ and \f$(\rho,\phi)\f$ planes (meters/radian),
 *   - \f$ ( s_\theta,s_\phi ) \f$
 *     = slope in the \f$(\rho,\theta)\f$ and \f$(\rho,\phi)\f$ planes (meters/meter),
 *   - \f$ ( \sigma_\theta,\sigma_\phi ) \f$
 *     = slope angle in the \f$(\rho,\theta)\f$ and \f$(\rho,\phi)\f$ planes (radians), and
 *   - \f$ ( n_\rho,n_\theta,n_\phi ) \f$
 *     = unit normal components in "rho","theta", and "phi" directions (meters).
 *
 * This definition of the unit normal saves processing time during
 * reflection processing.
 */
class USML_DECLSPEC boundary_model : public reflect_loss_model {

    //**************************************************
    // height model

  public:

    /**
     * Compute the height of the boundary and it's surface normal at
     * a series of locations.
     *
     * @param location      Location at which to compute boundary.
     * @param rho           Surface height in spherical earth coords (output).
     * @param normal        Unit normal relative to location (output).
     * @param quick_interp  Determines if you want a fast nearest or pchip interp
     */
    virtual void height( const wposition& location,
        matrix<double>* rho, wvector* normal=NULL, bool quick_interp=false ) = 0 ;

    /**
     * Compute the height of the boundary and it's surface normal at
     * a single location.  Often used during reflection processing.
     *
     * @param location      Location at which to compute boundary.
     * @param rho           Surface height in spherical earth coords (output).
     * @param normal        Unit normal relative to location (output).
     * @param quick_interp  Determines if you want a fast nearest or pchip interp
     */
    virtual void height( const wposition1& location,
        double* rho, wvector1* normal=NULL, bool quick_interp=false ) = 0 ;

    //**************************************************
    // reflection loss model

    /**
     * Define a new reflection loss model.
     *
     * @param reflect_loss   Reflection loss model.
     */
    void reflect_loss( reflect_loss_model* reflect_loss ) {
        if ( _reflect_loss_model ) delete _reflect_loss_model ;
        _reflect_loss_model = reflect_loss ;
    }

   /**
     * Computes the broadband reflection loss and phase change.
     *
     * @param location      Location at which to compute attenuation.
     * @param frequencies   Frequencies over which to compute loss. (Hz)
     * @param angle         Grazing angle relative to the interface (radians).
     * @param amplitude     Change in ray strength in dB (output).
     * @param phase         Change in ray phase in dB (output).
     */
    virtual void reflect_loss(
        const wposition1& location,
        const seq_vector& frequencies, double angle,
        vector<double>* amplitude, vector<double>* phase=NULL )
    {
        _reflect_loss_model->reflect_loss( location,
            frequencies, angle, amplitude, phase ) ;
    }

    /**
     * Computes the broadband reflection loss and phase change for
     * multiple locations.
     *
     * @param location      Location at which to compute attenuation.
     * @param frequencies   Frequencies over which to compute loss. (Hz)
     * @param angle         Reflection angle relative to the normal (radians).
     * @param amplitude     Change in ray strength in dB (output).
     * @param phase         Change in ray phase in radians (output).
     *                      Phase change not computed if this is NULL.
     * @param linear        returns the value back in linear or log units.
     */
    virtual void reflect_loss( const wposition& location,
        const seq_vector& frequencies, vector<double>* angle,
        vector<vector<double> >* amplitude,
        vector<vector<double> >* phase=NULL, bool linear=false )
    {
        _reflect_loss_model->reflect_loss( location,
            frequencies, angle, amplitude, phase, linear ) ;
    }

    /**
     * Setter for the scattering_model.
     * @param scatter   Scattering model for this boundary
     */
    void setScattering_Model( scattering_model* scatter ) {
        if( _scattering_model ) delete _scattering_model ;
        _scattering_model = scatter ;
    }

    /**
     * Getter for the scattering model of this boundary.
     * @return  pointer to the scattering_model
     */
    inline scattering_model* getScattering_Model() {
        return _scattering_model ;
    }

    //**************************************************
    // initialization

    /**
     * Initialize reflection loss components for a boundary.
     *
     * @param reflect_loss  Reflection loss model.
     */
    boundary_model( reflect_loss_model* reflect_loss=NULL,
                    scattering_model* scatter=NULL ) :
        _reflect_loss_model( reflect_loss ),
        _scattering_model( scatter )
    {
    }

    /**
     * Delete reflection loss model.
     */
    virtual ~boundary_model() {
        if ( _reflect_loss_model ) delete _reflect_loss_model ;
        if ( _scattering_model ) delete _scattering_model ;
    }

  protected:

    /** Reference to the reflection loss model **/
    reflect_loss_model* _reflect_loss_model ;

    /** Reference to the scattering strength model **/
    scattering_model* _scattering_model ;

};

/// @}
}  // end of namespace ocean
}  // end of namespace usml

#endif
