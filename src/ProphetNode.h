#pragma once

#ifndef __PROPHET_NODE_H__
#define __PROPHET_NODE_H__

#include "DutyCycle.h"

class CProphetNode :
	virtual public CNode
{
private:

	map<int, double> deliveryPreds;  //< ID:x, P(this->id, x) >��sink�ڵ�IDΪ0��λ����ǰ�����ڲ���

	CProphetNode(double dataRate);


public:

	virtual ~CProphetNode();

	//void init() override;

	static void initProphetNodes();

	static CProphetNode& toProphetNode(CNode& node);

	static vector<CProphetNode &>& getProphetNodes();

	void updateStatus(int currentTime) override;

	map<int, double> getDeliveryPreds() const
	{
		return deliveryPreds;
	}

	void setDeliveryPreds(map<int, double> deliveryPreds)
	{
		this->deliveryPreds = deliveryPreds;
	}

	CFrame* sendRTSWithCapacityAndPred(int currentTime);

	void initDeliveryPreds();
	void decayDeliveryPreds(int currentTime);
	void updateDeliveryPredsWith(int fromNode, map<int, double> preds);
	void updateDeliveryPredsWithSink(CSink* sink);

	bool shouldForward(map<int, double> dstPred);

	// TODO: check hop
	//select proper data to send
	vector<CData> getDataForTrans();

	vector<CData> bufferData(vector<CData> datas, int time);


};

#endif // __PROPHET_NODE_H__