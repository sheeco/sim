#pragma once
#include "Process.h"
#include "Node.h"
#include "Position.h"

class CTracePrediction :
	virtual public CProcess
{
private:

	class CPanSystem :
		virtual public CGeoEntity
	{
	private:
		CCoordinate delta;

	public:
		void setPan(CCoordinate delta)
		{
			this->delta = delta;
		}
		static CPanSystem readPanSystemFromFile(string filename)
		{
			try
			{
				if( !CFileHelper::IfExists(filename) )
				{
					throw pair<int, string>(EFILE, string("CPanSystem::readPanSystemFromFile() : Cannot find file \"" + filename + "\" ! "));
				}
				FILE *file;
				file = fopen(filename.c_str(), "rb");

				double temp_x = 0;
				double temp_y = 0;
				fscanf(file, "%lf %lf", &temp_x, &temp_y);
				fclose(file);

				CPanSystem pan;
				pan.setPan(CCoordinate(temp_x, temp_y));
				return pan;
			}
			catch( exception e )
			{
				throw string("CPanSystem::readPanSystemFromFile() : ") + e.what();
			}
		}
		CCoordinate applyPanding(CCoordinate coor)
		{
			return coor + delta;
		}
		CCoordinate cancelPanding(CCoordinate coor)
		{
			return coor - delta;
		}
		void CancelPanding(CCTrace* trace)
		{
			map<int, CCoordinate> coors = trace->getTrace();
			for( map<int, CCoordinate>::iterator icoor = coors.begin(); icoor != coors.end(); ++icoor )
			{
				icoor->second = cancelPanding(icoor->second);
			}
			trace->setTrace(coors);
		}
	};

	CNode *node = nullptr;
	CCTrace *predictions = nullptr;
	// FIXME: 1 or 30 ?

	static string KEYWORD_PREDICT;

	//e.g. 31.full.trace
	static string filenamePrediction(string nodename)
	{
		if( KEYWORD_PREDICT.empty() )
			KEYWORD_PREDICT = getConfig<string>("pferry", "keyword_predict");
		string filename = nodename + KEYWORD_PREDICT + getConfig<string>("trace", "extension_trace_file");
		return filename;
	}
	//e.g. 31.pan
	static string filenamePan(string nodename)
	{
		string filename = nodename + getConfig<string>("pferry", "extension_pan_file");
		return filename;
	}

public:

	CTracePrediction();
	~CTracePrediction();

	CTracePrediction(CNode *node, string dir): node(node)
	{
		string path = dir + '/' + filenamePrediction(this->node->getIdentifier());
		if( !CFileHelper::IfExists(path) )
		{
			throw string("CTracePredict::CTracePredict(): Cannot find trace file \"" + path + "\".");
		}
		else
		{
			this->predictions = CCTrace::readTraceFromFile(path, false);
			
			string pathPan = dir + '/' + filenamePan(this->node->getIdentifier());
			CPanSystem pan = CPanSystem::readPanSystemFromFile(pathPan);
			pan.CancelPanding(this->predictions);

			double hitrate = calculateHitrate(node->getTrace(), *(this->predictions), 100);
			CPrintHelper::PrintBrief("Trace prediction for " + node->getName() + " is loaded, hitrate " 
									  + STRING( NDigitFloat(hitrate * 100, 1) ) + "%.");
		}
	}

	int getStartTime() const
	{
		return this->predictions->getRange().first;
	}

	static double calculateHitrate(CCTrace fact, CCTrace pred, int hitrange)
	{
		map<int, CCoordinate> facts = fact.getTrace(), preds = pred.getTrace();
		int nHit = 0;
		for( pair<int, CCoordinate> pPred : preds )
		{
			if( CCoordinate::getDistance(pPred.second, facts[pPred.first]) <= hitrange )
				++nHit;
		}
		return double(nHit) / preds.size();
	}
	bool isValid(int time)
	{
		return predictions->isValid(time);
	}
	bool hasPrediction(int time)
	{
		return predictions->hasEntry(time);
	}
	CPosition getPrediction(int time)
	{
		if( !hasPrediction(time) )
			throw string("CTracePrediction::getPrediction(): Cannot find prediction for " 
						 + this->node->getName() + " at " + STRING(time) + "s.");
		return CPosition(this->node->getID(), predictions->getLocation(time), time);
	}
};

