#include "interface/WaveformNew.h"

#include "TGraph.h"
#include "TF1.h"
#include "TMath.h"
#include <assert.h>

#define WARNING_ERROR 0

using namespace std;

float WaveformNew::charge_integrated(const int& x1, const int& x2, float pedestal) const
{

  float return_value = 0;

  if (x1<0 || x2>( int(_samples.size())-1))
    {
      if (WARNING_ERROR)
	std::cout << "WARNING::charge_integrated::gate is outside samples range" << std::endl;
      return return_value;
    }
  
  for (unsigned int i(x1); i<=unsigned(x2); i++){
    return_value+=_samples[i]-pedestal;
  }
  
  return return_value;

}

float WaveformNew::amplitude_times_rms( const int &x1, const int &x2, float pedestal) const 
{
	float return_value=0;
	max_amplitude_informations max_ampl=max_amplitude(x1,x2,pedestal);
	return return_value;
}

WaveformNew::max_amplitude_informations WaveformNew::max_amplitude(const int& x1, const int& x2, int nSamplesAroundMax) const
{
  max_amplitude_informations return_value;  


  if (x1<0 || x2>(int(_samples.size())-1))
    {
      if (WARNING_ERROR)
	std::cout << "WARNING::WaveformNew::max_amplitude::gate is outside samples range" << std::endl;
      return return_value;
    }

  int imax=-1;
  float max=-999999.;

  for (unsigned int i(x1);i<=unsigned(x2);++i)
    {
      //      std::cout << "++++ " <<  i << "," << _times[i] << "," << _samples[i] << "," << max << "," << imax << std::endl;
      if (_samples[i]>=max)
	{
	  max=_samples[i];
	  imax=i;
	}
    }


  if (imax>-1)
    {
      float x[nSamplesAroundMax];
      float y[nSamplesAroundMax];
      int nSamples=0;
      for (unsigned int i(0);i<unsigned(nSamplesAroundMax);++i)
	{
	  if ( (imax-(nSamplesAroundMax-1)/2+int(i))>=x1 && (imax-(nSamplesAroundMax-1)/2+int(i))<=x2)
	    {
	      //	      x[i]=imax-(nSamplesAroundMax-1)/2+i;
	      x[i]=_times[imax-(nSamplesAroundMax-1)/2+i]*1e9;
	      y[i]=_samples[imax-(nSamplesAroundMax-1)/2+i];
	      //	      std::cout <<  imax << "," << imax-(nSamplesAroundMax-1)/2+i << "," << x[i] << "," << y[i] << std::endl;
      
	      ++nSamples;
	    }
	  else
	    {
	      if (WARNING_ERROR)
		std::cout << "WARNING::WaveformNew::max_amplitude::maximum found too close to gate edges. Increase gate width" << std::endl;
	    }
	}

      if (nSamples>3)
	{
	  //Now fit with parabolic function around maximum value
	  TGraph* graph=new TGraph(nSamples,x,y);
	  graph->Fit("pol2","Q0+");

	  //FIXME Add a check on the FIT status
	  double *par=graph->GetFunction("pol2")->GetParameters();
	  return_value.max_amplitude=par[0]-(par[1]*par[1]/(4*par[2]));
	  return_value.time_at_max=-(par[1]/(2*par[2]))/1.e9;
	  return_value.sample_at_max=imax;

	  delete graph;
	}
      else
	{
	  if (WARNING_ERROR)
	    std::cout << "WARNING::WaveformNew::max_amplitude::not enough samples to fit fot maximum. Returning unfitted position" << std::endl;
	  return_value.max_amplitude=max;
	  return_value.time_at_max=_times[imax];
	  return_value.sample_at_max=imax;
	}

    }
  return return_value;
};


//Get the residual value between a sample and expected value from interpolating samples
float WaveformNew::interpolatedValue(const int& i1, int SampleToInterpolate) const
{
  int cfSample=i1;

  //  std::cout <<  "++++ "  << cfSample << std::endl;
  if (cfSample>-1)
    {
      double x[SampleToInterpolate];
      double y[SampleToInterpolate];
      double syy=0,sxy=0,sxx=0,sx=0,sy=0;
      int nSamples=0;
      for (unsigned int i(0);i<unsigned(SampleToInterpolate);++i)
	{
	  if ( (cfSample-(SampleToInterpolate-1)/2+int(i))>=0 && (cfSample-(SampleToInterpolate-1)/2+int(i))<=(int)_samples.size() )
	    {
	      //	      x[i]=cfSample-(SampleToInterpolate-1)/2+i;
	      x[i]=(_times[cfSample-(SampleToInterpolate-1)/2+i] - _times[cfSample])*1e9 ;
	      y[i]=_samples[cfSample-(SampleToInterpolate-1)/2+i];
	      //std::cout <<  cfSample << "," << cfSample-(SampleToInterpolate-1)/2+i << "," << x[i] << "," << y[i] << std::endl;
	      syy+=y[i]*y[i];
	      sxy+=x[i]*y[i];
	      sxx+=x[i]*x[i];
	      sy+=y[i];
	      sx+=x[i];
      	      ++nSamples;
	    }
	  else
	    {
	      if (WARNING_ERROR)
		std::cout << "WARNING::WaveformNew::max_amplitude::maximum found too close to gate edges. Increase gate width" << std::endl;
	    }
	}

      if (nSamples>1)
	{
// 	  //Now fit with parabolic function around maximum value
// 	  TGraph* graph=new TGraph(nSamples,x,y);
// 	  graph->Fit("pol1","Q0+");

// 	  //FIXME Add a check on the FIT status
// 	  double *par=graph->GetFunction("pol1")->GetParameters();

	  double b = (nSamples*sxy-sx*sy)/(double) (nSamples*sxx - sx*sx);
	  double a = (sy - b*sx)/(double) nSamples;
	  return a;
	  
// 	  return -(par[0]/par[1])/1.e9;
// 	  delete graph;
	}
      else
	{
	  if (WARNING_ERROR)
	    std::cout << "WARNING::WaveformNew::max_amplitude::not enough samples to interpolate. Returning -999." << std::endl;
	  return -999.;
	}

    }
    assert(0);
    return -999.;
};


//Get the time at a given fraction of the amplitude for times between x1 and x2 
float WaveformNew::time_at_frac(const float& t1, const float& t2, const float& frac, const max_amplitude_informations& maxInfos, int SampleToInterpolate,bool rising) const
{
  //std::cout << "_____ " << t1 << "," << t2 << std::endl;
  int tmin=0;
  int tmax=0;
  for (unsigned int i(0);i<_times.size();++i)
    {
      //      std::cout << i << "," << _times[i] << std::endl;
      if (_times[i]<t1)
	tmin=i;
      if (_times[i]<t2)
	tmax=i;
    }

  return time_at_frac(tmin,tmax,frac,maxInfos,SampleToInterpolate,rising);
}


//Get the time at a given fraction of the amplitude for times between x1 and x2 
float WaveformNew::time_at_frac(const int& x1, const int& x2, const float& frac, const max_amplitude_informations& maxInfos, int SampleToInterpolate, bool rising) const
{
  int cfSample=-1;

  //  std::cout << "===== " << x1 << "," << x2 << std::endl;
  int start=(int)maxInfos.sample_at_max;
  int end1=max(x1,0);
  int end2=start;
  int incr=-1;
  if (not rising)
	{
	end1=start;
	end2=min( int(_samples.size())-1,x2);
	incr=+1;
	}
	

  for(int iSample=start; iSample>=end1 && iSample<=end2; iSample+=incr)
    {
      if(_samples[iSample] < maxInfos.max_amplitude*frac) 
	{
	  cfSample = iSample;
 	  break;
	}
    }
  
  //  std::cout <<  "++++ "  << cfSample << std::endl;
  if (cfSample>-1)
    {
      double x[SampleToInterpolate];
      double y[SampleToInterpolate];
      double syy=0,sxy=0,sxx=0,sx=0,sy=0;
      int nSamples=0;
      for (unsigned int i(0);i<unsigned(SampleToInterpolate);++i)
	{
	  if ( (cfSample-(SampleToInterpolate-1)/2+int(i))>=max(x1,0) && (cfSample-(SampleToInterpolate-1)/2+int(i))<=min(x2,(int)_samples.size()))
	    {
	      //	      x[i]=cfSample-(SampleToInterpolate-1)/2+i;
	      x[i]=_times[cfSample-(SampleToInterpolate-1)/2+i]*1e9;
	      y[i]=_samples[cfSample-(SampleToInterpolate-1)/2+i]-maxInfos.max_amplitude*frac;
	      //std::cout <<  cfSample << "," << cfSample-(SampleToInterpolate-1)/2+i << "," << x[i] << "," << y[i] << std::endl;
	      syy+=y[i]*y[i];
	      sxy+=x[i]*y[i];
	      sxx+=x[i]*x[i];
	      sy+=y[i];
	      sx+=x[i];
      	      ++nSamples;
	    }
	  else
	    {
	      if (WARNING_ERROR)
		std::cout << "WARNING::WaveformNew::max_amplitude::maximum found too close to gate edges. Increase gate width" << std::endl;
	    }
	}

      if (nSamples>1)
	{
// 	  //Now fit with parabolic function around maximum value
// 	  TGraph* graph=new TGraph(nSamples,x,y);
// 	  graph->Fit("pol1","Q0+");

// 	  //FIXME Add a check on the FIT status
// 	  double *par=graph->GetFunction("pol1")->GetParameters();

	  double b = (nSamples*sxy-sx*sy)/(double) (nSamples*sxx - sx*sx);
	  double a = (sy - b*sx)/(double) nSamples;
	  return -(a/b)/1.e9;
	  
// 	  return -(par[0]/par[1])/1.e9;
// 	  delete graph;
	}
      else
	{
	  if (WARNING_ERROR)
	    std::cout << "WARNING::WaveformNew::max_amplitude::not enough samples to fit fot maximum. Returning unfitted position" << std::endl;
	  return _times[cfSample];
	}

    }
    //assert(0); // should not arrive here
    return -999;
};

//Get the crossing time of the amplitude at a given threshold for times between t1 and t2 
std::vector<float> WaveformNew::time_at_threshold(const float& t1, const float& t2, const float& threshold, int SampleToInterpolate) const 
{
  //std::cout << "_____ " << t1 << "," << t2 << std::endl;
  int tmin=0;
  int tmax=0;
  for (unsigned int i(0);i<_times.size();++i)
    {
      //      std::cout << i << "," << _times[i] << std::endl;
      if (_times[i]<t1)
	tmin=i;
      if (_times[i]<t2)
	tmax=i;
    }

  return time_at_threshold(tmin,tmax,threshold,SampleToInterpolate);
}


//Get all the crossing times of the waveform at a given threshold for samples between x1 and x2 
std::vector<float> WaveformNew::time_at_threshold(const int& x1, const int& x2, const float& threshold, int SampleToInterpolate) const 
{
  int direction= 1 - 2*( _samples[max(x1,0)]<threshold ); //=-1 if first sample is below threshold, =1 if above
  
  std::vector<int> cfSample;
  
  for(int iSample=max(x1,0); iSample< min( (int) _samples.size() , x2); ++iSample)
    {
      if( (direction==-1 && (_samples[iSample] > threshold ) ) 
	  || (direction==1 && (_samples[iSample] < threshold ) ) ) 
	{
	  cfSample.push_back(iSample);
	  direction*=-1;
	}
    }

  std::vector<float> crossingTimes;

  for(int icross=0;icross<int(cfSample.size());++icross)
    {
      double x[SampleToInterpolate];
      double y[SampleToInterpolate];
      double syy=0,sxy=0,sxx=0,sx=0,sy=0;

      int nSamples=0;
      for (unsigned int i(0);i<unsigned(SampleToInterpolate);++i)
	{

	  if ( (cfSample[icross]-(SampleToInterpolate-1)/2+int(i))>=max(x1,0) && (cfSample[icross]-(SampleToInterpolate-1)/2+int(i))<=min(x2,(int)_samples.size()))
	    {
	      //	      x[i]=cfSample[icross]-(SampleToInterpolate-1)/2+i;
	      x[i]=_times[cfSample[icross]-(SampleToInterpolate-1)/2+i]*1e9;
	      y[i]=_samples[cfSample[icross]-(SampleToInterpolate-1)/2+i]-threshold;
	      //std::cout <<  cfSample[icross] << "," << cfSample[icross]-(SampleToInterpolate-1)/2+i << "," << x[i] << "," << y[i] << std::endl;
	      syy+=y[i]*y[i];
	      sxy+=x[i]*y[i];
	      sxx+=x[i]*x[i];
	      sy+=y[i];
	      sx+=x[i];
      	      ++nSamples;
	    }
	  else
	    {
	      if (WARNING_ERROR)
		std::cout << "WARNING::WaveformNew::max_amplitude::maximum found too close to gate edges. Increase gate width" << std::endl;
	    }
	}

      if (nSamples>1)
	{
// 	  //Now fit with parabolic function around maximum value
// 	  TGraph* graph=new TGraph(nSamples,x,y);
// 	  graph->Fit("pol1","Q0+");

// 	  //FIXME Add a check on the FIT status
// 	  double *par=graph->GetFunction("pol1")->GetParameters();

// 	  crossingTimes.push_back( -(par[0]/par[1])/1.e9 );
// 	  delete graph;
//        //Use the regression formula instead of the fit
	  double b = (nSamples*sxy-sx*sy)/(double) (nSamples*sxx - sx*sx);
	  double a = (sy - b*sx)/(double) nSamples;
	  crossingTimes.push_back( -(a/b)/1.e9 );
	}
      else
	{
	  if (WARNING_ERROR)
	    std::cout << "WARNING::WaveformNew::max_amplitude::not enough samples to fit fot maximum. Returning unfitted position" << std::endl;
	  crossingTimes.push_back( _times[cfSample[icross]] );
	}
      
    }
  return crossingTimes;
}

void WaveformNew::find_interesting_samples(int nSamples, const max_amplitude_informations& maxInfos, float riseTime, float fallTime, int& x1,int &x2)
{
  //std::cout << "_____ " << t1 << "," << t2 << std::endl;
  int tmin=0;
  int tmax=0;
  for (unsigned int i(0);i<_times.size();++i)
    {
      //      std::cout << i << "," << _times[i] << std::endl;
      if (_times[i]<maxInfos.time_at_max-2.*riseTime)
	tmin=i;
      if (_times[i]<maxInfos.time_at_max+1.*fallTime)
	tmax=i;
    }

  if ((tmax-tmin+1)==nSamples)
    {
      x1=tmin;
      x2=tmax;
    }
  else if ((tmax-tmin+1)<nSamples)
    {
      if (_samples.size()<unsigned(nSamples))
	{
	  x1=0;
	  x2=_samples.size()-1;
	}
      else if (tmin+nSamples-1<int(_samples.size()))
	{
	  x1=tmin;
	  x2=(tmin+nSamples-1);
	}
      else
	{
	  x2=_samples.size();
	  x1=x2-nSamples+1;
	}
    }
  else if ((tmax-tmin+1)>nSamples)
    {
      x1=tmin;
      x2=(tmin+nSamples-1);
    }
}
//Get the baseline (pedestal and RMS) informations computed between x1 and x2
WaveformNew::baseline_informations WaveformNew::baseline(const int& x1, const int& x2) const
{

  baseline_informations return_value;

  if (x1<0 || x2>(int(_samples.size())-1))
    {
      if (WARNING_ERROR)
	std::cout << "WARNING::WaveformNew::baseline::gate is outside samples range" << std::endl;
      return return_value;
    }
  
  if ((x2-x1)<2)
    {
      if (WARNING_ERROR)
	std::cout << "WARNING::WaveformNew::baseline::you need >2 samples to get pedestal & rms" << std::endl;
      return return_value;
    }

  double mean=0;
  double rms=0;

  for (unsigned int i(x1);i<=unsigned(x2);++i)
    {
      mean+=_samples[i];
      rms+=_samples[i]*_samples[i];
    }

  mean=mean/(double)(x2-x1+1);
  rms=TMath::Sqrt((x2-x1+1)/(x2-x1))*TMath::Sqrt( rms/(x2-x1+1) - mean*mean );
  return_value.pedestal = mean;
  return_value.rms = rms;

  return return_value;

};



//compute fft
void WaveformNew::fft(float lowCut,float upperCut){//CHECK if it's ok
  
  int N=_samples.size();//n samples of original waveform

  Double_t* input = sampleWaveform(N);


  TVirtualFFT* fftr2c = TVirtualFFT::FFT(1, &N, "R2CK");
  TVirtualFFT* fftc2r = TVirtualFFT::FFT(1, &N, "C2RK");

  fftr2c->SetPoints(input);
  fftr2c->Transform();

  // Loop over first half +1 of complex output results, multiply 
  // and set as input of reverse transform
  for (Int_t i=0 ; i<N/2+1; i++) {
    Double_t re,im ;
    fftr2c->GetPointComplex(i,re,im) ;
    //compute frequency in fft
    float Fs=N/(_samples[N-1]-_samples[0]);//sampling rate 
    float F=i*Fs/N;
    if(F<lowCut || F>upperCut) continue;
    TComplex t(re,im) ;
    fftc2r->SetPointComplex(i,t) ;
  }

  // Reverse Complex->Real FFT transform product
  fftc2r->Transform() ;


  WaveformNew* invfftWave = new WaveformNew();

  for (Int_t i =0 ; i<N ; i++) {
    _samples[i]=fftc2r->GetPointReal(i);
    //    invfftWave->addSample(fftr2c->GetPointReal(i),(_times[1]-_times[0]));

  }

}




Double_t* WaveformNew::sampleWaveform(int N){

  // Allocate array of sampling size plus optional buffer zones
  Double_t* array = new Double_t[N] ;


  // Scan function and store values in array
  for (Int_t i=0 ; i<N ; i++) {
    array[i] = _samples[i] ;
  }  
  
  return array ;


}




///multiply two waveforms
void WaveformNew::multiply(WaveformNew*  wave)
  {
    for (unsigned int i(0);i<_samples.size();++i){
      //      std::cout<<"[MULTIPLY]:"<<_samples[i]<<std::endl;
      _samples[i]*=wave->_samples[i];
      //      std::cout<<"samples:"<<_samples[i]<<" wave:"<<wave->_samples[i]<<std::endl;
    }
  };


//simulate uncorrelated noise on each sample
void WaveformNew::addUncorrNoise(float noise){//noise is in percentage of max ampl
  TRandom3* noiseGen = new TRandom3(time(NULL));
  WaveformNew::max_amplitude_informations wave_max_bare = max_amplitude(4,100,3);

  for (unsigned int i(0);i<_samples.size();++i){
    float noiseG=noiseGen->Gaus(0.,noise)*wave_max_bare.max_amplitude;
    _samples[i]+=noiseG;
  }  

}
