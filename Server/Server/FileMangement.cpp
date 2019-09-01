#include "function.h"

void ImportFile(vector<Account>& v) {
	// 플레이어 정보를 파일에서 가져온다

	// 컨테이너의 크기를 txt에서 가져옴
	ifstream inSize("AcccountNum.txt");
	int size;
	inSize >> size;
	v.reserve(size);

	// 컨테이너의 내용을 가져옴
	ifstream inData("PlayerDate.dat");
	Account ptr;
	while (inData >> ptr.id) {
		inData >> ptr.password;
		inData >> ptr.x;
		inData >> ptr.y;
		inData >> ptr.z;
		v.emplace_back(ptr);
	}
	inSize.close();
	inData.close();
}
void SaveFile(vector<Account>& v) {
	// 플레이어 정보를 파일로 저장

	// 먼저 컨테이너의 크기를 txt로 저장한다.
	ofstream outSize("AcccountNum.txt");
	unsigned int fsize{ v.size() };
	outSize << fsize;

	// 컨테이너의 내용을 저장
	ofstream outData("PlayerDate.dat");
	for (int i = 0; i < fsize; ++i) {
		outData << v[i].id << " " << v[i].password << " ";
		outData << v[i].x << " " << v[i].y << " "<<v[i].z<< endl;
	}

	outSize.close();
	outData.close();
}