struct Punkt
{
    float m;
    float v;
    float x;
};

struct PunktNormal
{
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float nz;
};

cbuffer Constants : register(b0)
{
    float dt;
    float w;
    int N_X;
    int N_Y;
    float zv;
    float czas;
}

[numthreads(1, 1, 1)]
void obliczWspolrzedne(
    RWStructuredBuffer<Punkt> aa : register(u0),
    RWStructuredBuffer<Punkt> bb : register(u1),
    uint3 threadID : SV_DispatchThreadID
)
{
    uint i = threadID.x;
    uint j = threadID.y;

    if (i > 0 && i < N_X - 1 && j > 0 && j < N_Y - 1)
    {
        float F = w * (4 * aa[i * N_Y + j].x
                     - aa[i * N_Y + (j + 1)].x - aa[i * N_Y + (j - 1)].x
                     - aa[(i + 1) * N_Y + j].x - aa[(i - 1) * N_Y + j].x);
        
        bb[i * N_Y + j].v = aa[i * N_Y + j].v + F / aa[i * N_Y + j].m * dt;
        bb[i * N_Y + j].x = aa[i * N_Y + j].x + bb[i * N_Y + j].v * dt;
        bb[i * N_Y + j].m = aa[i * N_Y + j].m;
    }

    if (czas < 12.56 && i == 50 && j == 50)
    {
        bb[5050].v = zv;
        bb[i * N_Y + j].m = aa[i * N_Y + j].m;
    }

    if (i == 0 || i == N_X - 1 || j == 0 || j == N_Y - 1)
    {
        bb[i * N_Y + j].x = 0.0;
        bb[i * N_Y + j].m = aa[i * N_Y + j].m;
    }
    
}

[numthreads(1, 1, 1)]
void obliczNormalne(
    RWStructuredBuffer<Punkt> bb : register(u0),
    RWStructuredBuffer<PunktNormal> punorm : register(u1),
    //int N_X,
    //int N_Y,
    uint3 threadID : SV_DispatchThreadID
)
{
    uint i = threadID.x;
    uint j = threadID.y;

    if (i > 0 && i < N_X - 1 && j > 0 && j < N_Y - 1)
    {
        float3 p0 = float3(i, j, bb[i * N_Y + j].x);
        float3 px1 = float3(i + 1, j, bb[(i + 1) * N_Y + j].x);
        float3 px2 = float3(i - 1, j, bb[(i - 1) * N_Y + j].x);
        float3 py1 = float3(i, j + 1, bb[i * N_Y + (j + 1)].x);
        float3 py2 = float3(i, j - 1, bb[i * N_Y + (j - 1)].x);

        float3 dx = px1 - px2;
        float3 dy = py1 - py2;

        float3 normal = normalize(cross(dx, dy));

        punorm[i * N_Y + j].x = i;
        punorm[i * N_Y + j].y = j;
        punorm[i * N_Y + j].z = bb[i * N_Y + j].x * 20;
        punorm[i * N_Y + j].nx = normal.x;
        punorm[i * N_Y + j].ny = normal.y;
        punorm[i * N_Y + j].nz = normal.z;
    }

    if (i == 0 || i == N_X - 1 || j == 0 || j == N_Y - 1)
    {
        punorm[i * N_Y + j].x = i;
        punorm[i * N_Y + j].y = j;
        punorm[i * N_Y + j].z = bb[i * N_Y + j].x * 20;
        punorm[i * N_Y + j].nx = 0.0;
        punorm[i * N_Y + j].ny = 0.0;
        punorm[i * N_Y + j].nz = 1.0;
    }
    
    //schader prawid³owo przekazuje do renderowania punkty, ale coœ Ÿle liczy!!! dla indeksów ponad 100 s¹ zera!!!!!
    punorm[0].x = 0;
    punorm[0].y = 0.5f;
    punorm[0].z = 0;
    punorm[0].nx = 0;
    punorm[0].ny = 0;
    punorm[0].nz = -1.0f;
    
    punorm[1].x = -0.5f;
    punorm[1].y = -0.5f;
    punorm[1].z = 0;
    punorm[1].nx = 0;
    punorm[1].ny = 0;
    punorm[1].nz = -1.0f;

    punorm[100].x = 0.5f;
    punorm[100].y = -0.5f;
    punorm[100].z = 0;
    punorm[100].nx = 0;
    punorm[100].ny = 0;
    punorm[100].nz = -1.0f;
    
    
    
}
