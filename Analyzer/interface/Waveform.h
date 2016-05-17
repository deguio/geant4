#ifndef WaveForm_h
#define WaveForm_h

#include "TROOT.h"
#include "Math/Interpolator.h"
#include "TVirtualFFT.h"

#include "TH1.h"

#include <vector> 
#include <algorithm> 
#include <iostream> 

class Waveform
{
 public:
  std::vector<double> _samples;
  std::vector<double> _times;
  ROOT::Math::Interpolator* _interpolator;
  // FFT --- members	
  std::vector<double> _fft_re;
  std::vector<double> _fft_im;

  std::vector<double> _fftI_samples;
	
  void fft(); // compute the fft
  void inv_fft(int cut,double tau=0.0001); // compute the inverse fft, with a freq cut. Hard.
  void inv_fft(); // compute the inverse fft, with a freq cut. Hard.
  void inv_fft_multiply(); // compute the inverse fft, with a freq cut. Hard.
  
  struct max_amplitude_informations
  {
    max_amplitude_informations(){ max_amplitude=-1; time_at_max=-1; sample_at_max=-1;}
    float max_amplitude;
    float time_at_max;
    int sample_at_max;
  };

  
  struct baseline_informations
  {
    baseline_informations(){ pedestal=-9999.; rms=-999.; };
    float pedestal;
    float rms;
  };

  Waveform() : _interpolator(0)
  {
  };
  
  //constructor from float array
  Waveform(int nSamples, float* times, float* samples) : _interpolator(0)
  {
    _samples.resize(nSamples);
    _times.resize(nSamples);
      for (unsigned int i(0);i<unsigned(nSamples);++i)
	{
	  //	  std::cout<<"i:"<<i<<" "<<times[i]<<" "<<samples[i]<<std::endl;
	  _samples[i]=samples[i];
	  _times[i]=times[i];
	}
    };

  //constructor from double array
  Waveform(int nSamples, double* times, double* samples) : _interpolator(0)
  {
    _samples.resize(nSamples);
    _times.resize(nSamples);
      for (unsigned int i(0);i<unsigned(nSamples);++i)
	{
	  //	  std::cout<<"i:"<<i<<" "<<times[i]<<" "<<samples[i]<<std::endl;
	  _samples[i]=samples[i];
	  _times[i]=times[i];
	}
    };



  //constructor from std::vector<float>
  Waveform(const std::vector<float>& times, const std::vector<float>& samples) : _interpolator(0)
    {
      _samples.resize(samples.size());
      _times.resize(samples.size());
      std::copy(samples.begin(), samples.end(), _samples.begin());
      std::copy(times.begin(), times.end(), _times.begin());
    };

  ~Waveform()
    {
      if (_interpolator)
	delete _interpolator;
    };

  //print
  void print(){
    for (unsigned int i(0);i<_samples.size();++i){
      std::cout<<"i:"<<i<<" time:"<<_times[i]<<" samples:"<<_samples[i]<<std::endl;
    }
  }

  //add samples
  void addSample(const float& sample, float sampleTimeSize=0.2e-9)
  {
    _samples.push_back(sample);
    if (_times.size()>0)
      _times.push_back(_times.back()+sampleTimeSize);
    else
      _times.push_back(0.);
  }
  
  void addTimeAndSample(const float& time, const float& sample)
  {
    _samples.push_back(sample);
    _times.push_back(time);
  }

  void interpolate()
  {
    if  (_interpolator)
      return;

    _interpolator=new ROOT::Math::Interpolator(_samples.size(), ROOT::Math::Interpolation::kCSPLINE);
    _interpolator->SetData(_times,_samples);
  }

  //open a window of nSamples centered keeping into account where is the maximum and some characteristics of the shape
  void find_interesting_samples(int nSamples, const max_amplitude_informations& maxInfos, float riseTime, float fallTime, int& x1,int &x2);

  //Get the max amplitude between x1 and x2 using nSamplesAroundMax samples and a parabolic fit
  max_amplitude_informations max_amplitude(const int& x1, const int& x2, int nSamplesAroundMax=5) const;

  //Get the time at a given fraction of the amplitude for times between x1 and x2 
  float interpolatedValue(const int& i1, int SampleToInterpolate=9) const;

  //Get the time at a given fraction of the amplitude for times between x1 and x2 
  float time_at_frac(const int& i1, const int& i2, const float& frac, const max_amplitude_informations& maxInfos, int SampleToInterpolate=5,bool rising=true) const;

  //Get the time at a given fraction of the amplitude for times between x1 and x2 
  float time_at_frac(const float& t1, const float& t2, const float& frac, const max_amplitude_informations& maxInfos, int SampleToInterpolate=5,bool rising=true) const;

  //Get the baseline (pedestal and RMS) informations computed between x1 and x2
  baseline_informations baseline(const int& x1, const int& x2) const;

  //get values at crossing a specif threshold */
  std::vector<float> time_at_threshold(const float& t1, const float& t2, const float& threshold, int SampleToInterpolate=5) const; 

  //get values at crossing a specif threshold */
  std::vector<float> time_at_threshold(const int& x1, const int& x2, const float& threshold, int SampleToInterpolate=5) const; 

  //get value of integrated amplitude between x1 and x2, subtracting pedestal
  float charge_integrated(const int& x1, const int& x2, float pedestal=0) const;
  float amplitude_times_rms(const int& x1, const int& x2, float pedestal=0) const;
      
  float integral(const int& x1, const int& x2) const
  {
    float integral=0;
    for(int bin=x1; bin<x2; bin++)
      integral += _samples[bin];
    return integral;
  };

  //clear
  void clear()
  {
    _samples.clear();
    _times.clear();
    if (_interpolator)
      delete _interpolator;
  };

  //substract a fixed value from the samples
  void offset(const float& value)
  {
    for (unsigned int i(0);i<_samples.size();++i)
      _samples[i]-=value;
  };

  //rescale all the samples by a given rescale factor
  void rescale(const float& rescale_factor)
  {
    for (unsigned int i(0);i<_samples.size();++i)
      _samples[i]*=rescale_factor;
  };

  ///multiply two waveforms
  void multiply(Waveform*  wave)
  {
    for (unsigned int i(0);i<_samples.size();++i){
      //      std::cout<<"[MULTIPLY]:"<<_samples[i]<<std::endl;
      _samples[i]*=wave->_samples[i];
      //      std::cout<<"samples:"<<_samples[i]<<" wave:"<<wave->_samples[i]<<std::endl;
    }
  };

  ///multiply fft of two waveforms
  void multiply_fft(Waveform*  wave)//FIXME implement correctly
  {

    int n=_samples.size();
    //	TVirtualFFT *vfft =TVirtualFFT::FFT(1,&n,"C2CFORWARD");
    float bufferFraction=0.10;//buffer for avoiding cyclical flow, default is 10% of window
    int nbuf=(n*bufferFraction)/2+0.5;
    int N2=n+2*nbuf;
    
    std::cout<<"multiply n:"<<n<<" n2"<<N2<<" nbuf:"<<nbuf<<"fft_size: "<<_fft_re.size()<<std::endl;
    //    _fft_re.resize(_samples.size());
    //    _fft_im.resize(_samples.size());

    //    for (unsigned int i(0);i<_samples.size()/2+1;++i){
    for (unsigned int i(0);i<N2/2+1;++i){
	_fft_re[i]=_fft_re[i]*wave->_fft_re[i]-_fft_im[i]*wave->_fft_im[i];
	_fft_im[i]=_fft_re[i]*wave->_fft_im[i]+wave->_fft_re[i]*_fft_im[i];
//	std::cout<<"real"<<_fft_re[i]<<"wave:"<<wave->_fft_re[i]<<std::endl;
//	std::cout<<"im"<<_fft_im[i]<<"wave:"<<wave->_fft_im[i]<<std::endl;
    }
    _fft_re.resize(N2/2+1);
    _fft_im.resize(N2/2+1);

  };

  TH1F* get_histo(TString name){
    TH1F* histo=new TH1F(name,name,_times.size(),_times[0],_times[_samples.size()-1]);
    for (unsigned int i(0);i<_samples.size();++i){
      histo->Fill(_times[i],_samples[i]);
      std::cout<<"[GETHISTO]:"<<i<<" "<<_times[i]<<" "<<_samples[i]<<std::endl;
    }
    return histo;
  }

  TH1F* get_histo_inv_fft(TString name){
    TH1F* histo=new TH1F(name,name,_times.size()*2,_times[0]*2,_times[_samples.size()-1]*2);
    for (unsigned int i(0);i<_samples.size();++i){
      histo->Fill(_times[i],_samples[i]);
      std::cout<<"[GETHISTO]:"<<i<<" "<<_times[i]<<" "<<_samples[i]<<std::endl;
    }
    return histo;
  }


  //shift all samples by a given amount in time */
  void shift_time(const float& time_offset)
  {
    for (unsigned int i(0);i<_times.size();++i)
      _times[i]+=time_offset;
  } 
	
  int find_time_bin(const float& t1)
  {
  int tmin=0;
  for (unsigned int i(0);i<_times.size();++i)
    {
      //      std::cout << i << "," << _times[i] << std::endl;
      if (_times[i]<t1)
	tmin=i;
    }
  return tmin;
  }

  
    
};

#endif
