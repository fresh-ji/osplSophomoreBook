
#include <iostream>
#include <string>
#include <Windows.h>

#include "dds_service.h"

bool process(const MsgData& msgdata) {
	cout << "=======receive data:===========" << endl;
	cout << "*********************" << endl;
	cout << "subjectId:" << msgdata.subjectId << endl;
	cout << "time:" << msgdata.time << endl;
	cout << "from:" << msgdata.from << endl;
	cout << "topicName:" << msgdata.topicName << endl;
	cout << "text:" << msgdata.text << endl;
	cout << "data:" << msgdata.data << endl;
	cout << "*********************" << endl;
	return true;
}

void main() {

	CSDDSService *p_ddsInst = CSDDSService::Instance();
	p_ddsInst->Init("example");

	p_ddsInst->CreateTopic("A2B");
	p_ddsInst->CreateWriter("A2B");

	p_ddsInst->CreateTopic("B2A");
	p_ddsInst->CreateReader("B2A");

	function<bool(MsgData)> cb = bind(process, placeholders::_1);
	p_ddsInst->SetCallBack(cb);
	p_ddsInst->StartReceiveData();

	MsgData msgdata;
	msgdata.data = 3.0;
	p_ddsInst->write("A2B", msgdata);

	cout << "data sent" << endl;

	while (1) {
		Sleep(80);
	}
}
