#include "Mesh.h"



CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}


CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer)m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer)m_pd3dVertexUploadBuffer->Release();

	if (m_pd3dIndexBuffer)m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer)m_pd3dIndexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dVertexUploadBuffer)m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer)m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA : 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다.
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

CTriangleMesh::CTriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
	:CMesh(pd3dDevice, pd3dCommandList)
{
	//삼각형 메쉬를 정의한다.
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*정점(삼각형의 꼭지점)의 색상은 시계방향 순서대로 빨간색, 녹색, 파란색으로 지정한다. RGBA
	(Red, Green, Blue, Alpha) 4개의 파라메터를 사용하여 색상을 표현한다. 각 파라메터는 0.0~1.0 사이의 실수값을
	가진다.*/
	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 1.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(1.5f, -1.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-1.5f, -1.5f, 0.0f), XMFLOAT4(Colors::Blue));

	//삼각형 메쉬를 리소스(정점 버퍼)로 생성한다.
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//정점 버퍼 뷰를 생성한다.
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float fWidth, float fHeight, float fDepth) :CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다.
	m_nVertices = 8;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다.
	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	/*인덱스 버퍼는 직육면체의 6개의 면(사각형)에 대한 기하 정보를 갖는다. 삼각형 리스트로 직육면체를 표현할
	것이므로 각 면은 2개의 삼각형을 가지고 각 삼각형은 3개의 정점이 필요하다. 즉, 인덱스 버퍼는 전체 36(=6*2*3)개의
	인덱스를 가져야 한다*/
	m_nIndices = 36;
	UINT pnIndices[36];

	//앞면
	pnIndices[0] = 0; pnIndices[1] = 5; pnIndices[2] = 4;
	pnIndices[3] = 1; pnIndices[4] = 5; pnIndices[5] = 0;
	//윗면
	pnIndices[6] = 2; pnIndices[7] = 1; pnIndices[8] = 3;
	pnIndices[9] = 3; pnIndices[10] = 1; pnIndices[11] = 0;
	//뒷면
	pnIndices[12] = 2; pnIndices[13] = 7; pnIndices[14] = 6;
	pnIndices[15] = 3; pnIndices[16] = 7; pnIndices[17] = 2;
	//아랫면
	pnIndices[18] = 6; pnIndices[19] = 4; pnIndices[20] = 5;
	pnIndices[21] = 7; pnIndices[22] = 4; pnIndices[23] = 6;
	//왼쪽
	pnIndices[24] = 0; pnIndices[25] = 4; pnIndices[26] = 3;
	pnIndices[27] = 3; pnIndices[28] = 4; pnIndices[29] = 7;
	//오른쪽
	pnIndices[30] = 2; pnIndices[31] = 6; pnIndices[32] = 1;
	pnIndices[33] = 1; pnIndices[34] = 6; pnIndices[35] = 5;

	//인덱스 버퍼를 생성한다.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	//인덱스 버퍼 뷰를 생성한다.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{

}

CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	float fWidth, float fHeight, float fDepth, XMFLOAT4 xmf4Color) :CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = 24 * 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	//정점 데이터
	CDiffusedVertex pVertices[25 * 3];
	XMFLOAT4 AirplaneColor{ 1.0f,1.0f,1.0f,0.0f };

	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;

	//비행기 메쉬의 위쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	//비행기 메쉬의 아래쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	//비행기 메쉬의 오른쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	//비행기 메쉬의 뒤쪽/오른쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	//비행기 메쉬의 왼쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), -fz),
		Vector4::Add(xmf4Color, AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, +(fy + y3), +fz),
		Vector4::Add(xmf4Color, AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	//비행기 메쉬의 뒤쪽/왼쪽 면
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4Color,
		AirplaneColor));
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4Color,
		AirplaneColor));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
{

}