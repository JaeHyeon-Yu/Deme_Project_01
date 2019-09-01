#include "function.h"

void ImportFile(vector<Account>& v) {
	// �÷��̾� ������ ���Ͽ��� �����´�

	// �����̳��� ũ�⸦ txt���� ������
	ifstream inSize("AcccountNum.txt");
	int size;
	inSize >> size;
	v.reserve(size);

	// �����̳��� ������ ������
	ifstream inData("PlayerDate.dat");
	Account ptr;
	while (inData >> ptr.id) {
		inData >> ptr.password;
		inData >> ptr.x;
		inData >> ptr.y;
		v.emplace_back(ptr);
	}
	inSize.close();
	inData.close();
}
void SaveFile(vector<Account>& v) {
	// �÷��̾� ������ ���Ϸ� ����

	// ���� �����̳��� ũ�⸦ txt�� �����Ѵ�.
	ofstream outSize("AcccountNum.txt");
	unsigned int fsize{ v.size() };
	outSize << fsize;

	// �����̳��� ������ ����
	ofstream outData("PlayerDate.dat");
	for (int i = 0; i < fsize; ++i) {
		outData << v[i].id << " " << v[i].password << " ";
		outData << v[i].x << " " << v[i].y << endl;
	}

	outSize.close();
	outData.close();
}