typedef struct {
    float m, v, x;
} Punkt;

typedef struct {
    float x, y, z, nx, ny, nz;
} PunktNormal;

__kernel void obliczWspolrzedne(__global Punkt* aa, __global Punkt* bb, float dt, float w, int N_X, int N_Y, float zv, float czas) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    if (i > 0 && i < N_X-1 && j > 0 && j < N_Y-1) {
        float F = w * (4 * aa[i * N_Y + j].x
                     - aa[i * N_Y + (j + 1)].x - aa[i * N_Y + (j - 1)].x
                     - aa[(i + 1) * N_Y + j].x - aa[(i - 1) * N_Y + j].x);
        
        bb[i * N_Y + j].v = aa[i * N_Y + j].v + F / aa[i * N_Y + j].m * dt;      
        bb[i * N_Y + j].x = aa[i * N_Y + j].x + bb[i * N_Y + j].v * dt;
        bb[i * N_Y + j].m = aa[i * N_Y + j].m;
    }

    if (czas < 12.56 && i==50 && j==50) {
         bb[5050].v=zv;
    }

    if (i == 0 || i == N_X-1 || j == 0 || j == N_Y-1) {
        bb[i * N_Y + j].x = 0.0;
    }
}

__kernel void obliczNormalne(__global Punkt* bb, __global PunktNormal* punorm, int N_X, int N_Y) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    if (i > 0 && i < N_X-1 && j > 0 && j < N_Y-1) {
        float3 p0 = (float3)(i, j, bb[i * N_Y + j].x);
        float3 px1 = (float3)(i + 1, j, bb[(i + 1) * N_Y + j].x);
        float3 px2 = (float3)(i - 1, j, bb[(i - 1) * N_Y + j].x);
        float3 py1 = (float3)(i, j + 1, bb[i * N_Y + (j + 1)].x);
        float3 py2 = (float3)(i, j - 1, bb[i * N_Y + (j - 1)].x);

        float3 dx = px1 - px2;
        float3 dy = py1 - py2;

        float3 normal = cross(dx, dy);
        normal = normalize(normal);

        punorm[i * N_Y + j].x = i - 40.0f;
        punorm[i * N_Y + j].y = j - 40.0f;
        punorm[i * N_Y + j].z = bb[i * N_Y + j].x*20;
        punorm[i * N_Y + j].nx = normal.x;
        punorm[i * N_Y + j].ny = normal.y;
        punorm[i * N_Y + j].nz = normal.z;
    }

    if (i == 0 || i == N_X-1 || j == 0 || j == N_Y-1) {
        punorm[i * N_Y + j].x = i - 40.0f;
        punorm[i * N_Y + j].y = j - 40.0f;
        punorm[i * N_Y + j].z = bb[i * N_Y + j].x*20;
        punorm[i * N_Y + j].nx = 0.0;
        punorm[i * N_Y + j].ny = 0.0;
        punorm[i * N_Y + j].nz = 1.0;
    }
}

__kernel void przygotujTrojkaty(__global PunktNormal* wieszcholki, __global float* vertices, int N_X, int N_Y) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    if (i < N_Y - 1 && j < N_X - 1) {
        int index = (i * (N_X - 1) + j) * 36; // 12 vertices per square, 3 coordinates per vertex

        // First triangle
        vertices[index] = wieszcholki[i * N_Y + j].x - 40.0f;
        vertices[index + 1] = wieszcholki[i * N_Y + j].y - 40.0f;
        vertices[index + 2] = wieszcholki[i * N_Y + j].z;
        vertices[index + 3] = wieszcholki[i * N_Y + j].nx;
        vertices[index + 4] = wieszcholki[i * N_Y + j].ny;
        vertices[index + 5] = wieszcholki[i * N_Y + j].nz;

        vertices[index + 6] = wieszcholki[i * N_Y + (j + 1)].x - 40.0f;
        vertices[index + 7] = wieszcholki[i * N_Y + (j + 1)].y - 40.0f;
        vertices[index + 8] = wieszcholki[i * N_Y + (j + 1)].z;
        vertices[index + 9] = wieszcholki[i * N_Y + (j + 1)].nx;
        vertices[index + 10] = wieszcholki[i * N_Y + (j + 1)].ny;
        vertices[index + 11] = wieszcholki[i * N_Y + (j + 1)].nz;

        vertices[index + 12] = wieszcholki[(i + 1) * N_Y + j].x - 40.0f;
        vertices[index + 13] = wieszcholki[(i + 1) * N_Y + j].y - 40.0f;
        vertices[index + 14] = wieszcholki[(i + 1) * N_Y + j].z;
        vertices[index + 15] = wieszcholki[(i + 1) * N_Y + j].nx;
        vertices[index + 16] = wieszcholki[(i + 1) * N_Y + j].ny;
        vertices[index + 17] = wieszcholki[(i + 1) * N_Y + j].nz;

        // Second triangle
        vertices[index + 18] = wieszcholki[(i + 1) * N_Y + j].x - 40.0f;
        vertices[index + 19] = wieszcholki[(i + 1) * N_Y + j].y - 40.0f;
        vertices[index + 20] = wieszcholki[(i + 1) * N_Y + j].z;
        vertices[index + 21] = wieszcholki[(i + 1) * N_Y + j].nx;
        vertices[index + 22] = wieszcholki[(i + 1) * N_Y + j].ny;
        vertices[index + 23] = wieszcholki[(i + 1) * N_Y + j].nz;

        vertices[index + 24] = wieszcholki[i * N_Y + (j + 1)].x - 40.0f;
        vertices[index + 25] = wieszcholki[i * N_Y + (j + 1)].y - 40.0f;
        vertices[index + 26] = wieszcholki[i * N_Y + (j + 1)].z;
        vertices[index + 27] = wieszcholki[i * N_Y + (j + 1)].nx;
        vertices[index + 28] = wieszcholki[i * N_Y + (j + 1)].ny;
        vertices[index + 29] = wieszcholki[i * N_Y + (j + 1)].nz;

        vertices[index + 30] = wieszcholki[(i + 1) * N_Y + (j + 1)].x - 40.0f;
        vertices[index + 31] = wieszcholki[(i + 1) * N_Y + (j + 1)].y - 40.0f;
        vertices[index + 32] = wieszcholki[(i + 1) * N_Y + (j + 1)].z;
        vertices[index + 33] = wieszcholki[(i + 1) * N_Y + (j + 1)].nx;
        vertices[index + 34] = wieszcholki[(i + 1) * N_Y + (j + 1)].ny;
        vertices[index + 35] = wieszcholki[(i + 1) * N_Y + (j + 1)].nz;
    }
}


__kernel void przygotujTrojkaty_stare(__global PunktNormal* wieszcholki, __global float* vertices, int N_X, int N_Y) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    if (i < N_Y - 1 && j < N_X - 1) {
        int index = (i * (N_X - 1) + j) * 18; // 6 vertices per triangle, 3 coordinates per vertex

        // First triangle
        vertices[index] = wieszcholki[i * N_Y + j].x - 40.0f;
        vertices[index + 1] = wieszcholki[i * N_Y + j].y - 40.0f;
        vertices[index + 2] = wieszcholki[i * N_Y + j].z;
        vertices[index + 3] = wieszcholki[i * N_Y + j].nx;
        vertices[index + 4] = wieszcholki[i * N_Y + j].ny;
        vertices[index + 5] = wieszcholki[i * N_Y + j].nz;

        vertices[index + 6] = wieszcholki[i * N_Y + (j + 1)].x - 40.0f;
        vertices[index + 7] = wieszcholki[i * N_Y + (j + 1)].y - 40.0f;
        vertices[index + 8] = wieszcholki[i * N_Y + (j + 1)].z;
        vertices[index + 9] = wieszcholki[i * N_Y + (j + 1)].nx;
        vertices[index + 10] = wieszcholki[i * N_Y + (j + 1)].ny;
        vertices[index + 11] = wieszcholki[i * N_Y + (j + 1)].nz;

        vertices[index + 12] = wieszcholki[(i + 1) * N_Y + j].x - 40.0f;
        vertices[index + 13] = wieszcholki[(i + 1) * N_Y + j].y - 40.0f;
        vertices[index + 14] = wieszcholki[(i + 1) * N_Y + j].z;
        vertices[index + 15] = wieszcholki[(i + 1) * N_Y + j].nx;
        vertices[index + 16] = wieszcholki[(i + 1) * N_Y + j].ny;
        vertices[index + 17] = wieszcholki[(i + 1) * N_Y + j].nz;
    }
}
