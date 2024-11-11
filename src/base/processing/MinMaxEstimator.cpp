
#include "processing/MinMaxEstimator.hpp"

namespace signal_estimator {

    // called from output thread
void MinMaxEstimator::add_output(FramePtr frame){
    if(frame->has_impulse()){
        impulse_started_.store(true);
        impulse_time_.store(frame->sw_frame_time());
    }else{
        impulse_started_.store(false);
    }
    m_Estimator->add_output(frame);
};

    // called from input thread
void MinMaxEstimator::add_input(FramePtr frame){
    if(impulse_started_.load() &&  
        (frame->sw_frame_time() >= impulse_time_.load() + config_.min_latency) &&
        (frame->sw_frame_time() < impulse_time_.load() + config_.max_latency)){
            m_Estimator->add_input(frame);
        }
    else{
        for (size_t i = 0; i < frame->size(); i++){
            (frame->data())[i] = 0;
        }
        m_Estimator->add_input(frame);
    }
};


} // namespace signal_estimator