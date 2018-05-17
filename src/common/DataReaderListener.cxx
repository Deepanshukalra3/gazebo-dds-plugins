#ifndef DATA_READER_LISTENER_CXX
#define DATA_READER_LISTENER_CXX

#include "../include/DataReaderListener.h"

template <typename T>
DataReaderListener<T>::DataReaderListener(const std::function<void(T)> &on_data)
        : on_data_(on_data)
{
}

template <typename T>
void DataReaderListener<T>::on_data_available(dds::sub::DataReader<T> &reader)
{
    dds::sub::LoanedSamples<T> samples = reader.take();
    for (auto sample : samples) {
        // If the reference we get is valid data, it means we have actual
        // data available, otherwise we got metadata.
        if (sample.info().valid()) {
            on_data_();
        }
    }
}

#endif  // DATA_READER_LISTENER_CXX
