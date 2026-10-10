const COMPUTE_WGSL = /* wgsl */ `
struct Punkt {
    m : f32,
    v : f32,
    x : f32,
};

struct Constants {
    dt   : f32,
    w    : f32,
    nx   : u32,
    ny   : u32,
    zv   : f32,
    czas : f32,
    pad0 : f32,
    pad1 : f32,
};

struct PunktNormal {
    pos  : vec3<f32>,
    norm : vec3<f32>,
};

@group(0) @binding(0)
var<uniform> constants : Constants;

@group(0) @binding(1)
var<storage, read> aa : array<Punkt>;

@group(0) @binding(2)
var<storage, read_write> bb : array<Punkt>;

fn index(i : u32, j : u32) -> u32 {
    return i * constants.ny + j;
}

@compute
@workgroup_size(8, 8, 1)
fn obliczWspolrzedne(
    @builtin(global_invocation_id) threadId : vec3<u32>
) {
    let i = threadId.x;
    let j = threadId.y;

    if (i >= constants.nx || j >= constants.ny) {
        return;
    }

    let currentIndex = index(i, j);

    // Najpierw kopiujemy cały aktualny punkt.
    bb[currentIndex] = aa[currentIndex];

    if (
        i > 0u &&
        i < constants.nx - 1u &&
        j > 0u &&
        j < constants.ny - 1u
    ) {
        let xp = aa[index(i, j + 1u)].x;
        let xm = aa[index(i, j - 1u)].x;
        let yp = aa[index(i + 1u, j)].x;
        let ym = aa[index(i - 1u, j)].x;

        let f = constants.w * (
            4.0 * aa[currentIndex].x -
            xp -
            xm -
            yp -
            ym
        );

        let newVelocity =
            aa[currentIndex].v +
            f / aa[currentIndex].m * constants.dt;

        let newPosition =
            aa[currentIndex].x +
            newVelocity * constants.dt;

        bb[currentIndex].v = newVelocity;
        bb[currentIndex].x = newPosition;
        bb[currentIndex].m = aa[currentIndex].m;
    }

    // Źródło fali.
    // Dla mniejszych siatek środek jest wyliczany dynamicznie.
    let sourceI = constants.nx / 2u;
    let sourceJ = constants.ny / 2u;

    if (
        constants.czas < 12.56 &&
        i == sourceI &&
        j == sourceJ
    ) {
        bb[currentIndex].v = constants.zv;
        bb[currentIndex].m = aa[currentIndex].m;
    }

    // Stałe warunki brzegowe.
    if (
        i == 0u ||
        i == constants.nx - 1u ||
        j == 0u ||
        j == constants.ny - 1u
    ) {
        bb[currentIndex].x = 0.0;
        bb[currentIndex].v = 0.0;
        bb[currentIndex].m = aa[currentIndex].m;
    }
}
`;

const NORMALS_WGSL = /* wgsl */ `
struct Punkt {
    m : f32,
    v : f32,
    x : f32,
};

struct Constants {
    dt   : f32,
    w    : f32,
    nx   : u32,
    ny   : u32,
    zv   : f32,
    czas : f32,
    pad0 : f32,
    pad1 : f32,
};

struct PunktNormal {
    pos  : vec3<f32>,
    norm : vec3<f32>,
};

@group(0) @binding(0)
var<uniform> constants : Constants;

@group(0) @binding(1)
var<storage, read> points : array<Punkt>;

@group(0) @binding(2)
var<storage, read_write> vertices : array<PunktNormal>;

fn index(i : u32, j : u32) -> u32 {
    return i * constants.ny + j;
}

@compute
@workgroup_size(8, 8, 1)
fn obliczNormalne(
    @builtin(global_invocation_id) threadId : vec3<u32>
) {
    let i = threadId.x;
    let j = threadId.y;

    if (i >= constants.nx || j >= constants.ny) {
        return;
    }

    let currentIndex = index(i, j);
    let heightScale = 20.0;

    vertices[currentIndex].pos = vec3<f32>(
        f32(i),
        f32(j),
        points[currentIndex].x * heightScale
    );

    if (
        i > 0u &&
        i < constants.nx - 1u &&
        j > 0u &&
        j < constants.ny - 1u
    ) {
        let px1 = vec3<f32>(
            f32(i + 1u),
            f32(j),
            points[index(i + 1u, j)].x * heightScale
        );

        let px2 = vec3<f32>(
            f32(i - 1u),
            f32(j),
            points[index(i - 1u, j)].x * heightScale
        );

        let py1 = vec3<f32>(
            f32(i),
            f32(j + 1u),
            points[index(i, j + 1u)].x * heightScale
        );

        let py2 = vec3<f32>(
            f32(i),
            f32(j - 1u),
            points[index(i, j - 1u)].x * heightScale
        );

        let dx = px1 - px2;
        let dy = py1 - py2;

        var normal = normalize(cross(dx, dy));

        // Kierunek zależy od kolejności iloczynu wektorowego.
        if (normal.z < 0.0) {
            normal = -normal;
        }

        vertices[currentIndex].norm = normal;
    } else {
        vertices[currentIndex].norm = vec3<f32>(0.0, 0.0, 1.0);
    }
}
`;

const RENDER_WGSL = /* wgsl */ `
struct PunktNormal {
    pos  : vec3<f32>,
    norm : vec3<f32>,
};

struct RenderConstants {
    nx         : f32,
    ny         : f32,
    aspect     : f32,
    heightZoom : f32,

    rotationX  : f32,
    rotationZ  : f32,
    zoom       : f32,
    pad0       : f32,

    lightDir   : vec4<f32>,
    baseColor  : vec4<f32>,
};

struct VertexOutput {
    @builtin(position) position : vec4<f32>,
    @location(0) normal : vec3<f32>,
    @location(1) height : f32,
};

@group(0) @binding(0)
var<uniform> renderConstants : RenderConstants;

@group(0) @binding(1)
var<storage, read> vertices : array<PunktNormal>;

fn rotateX(p : vec3<f32>, angle : f32) -> vec3<f32> {
    let c = cos(angle);
    let s = sin(angle);

    return vec3<f32>(
        p.x,
        p.y * c - p.z * s,
        p.y * s + p.z * c
    );
}

fn rotateZ(p : vec3<f32>, angle : f32) -> vec3<f32> {
    let c = cos(angle);
    let s = sin(angle);

    return vec3<f32>(
        p.x * c - p.y * s,
        p.x * s + p.y * c,
        p.z
    );
}

@vertex
fn vsMain(
    @builtin(vertex_index) vertexIndex : u32
) -> VertexOutput {
    let vertex = vertices[vertexIndex];

    let center = vec3<f32>(
        (renderConstants.nx - 1.0) * 0.5,
        (renderConstants.ny - 1.0) * 0.5,
        0.0
    );

    let denominator = max(
        renderConstants.nx,
        renderConstants.ny
    );

    var p = vertex.pos - center;

    p.x = p.x / denominator * 2.0;
    p.y = p.y / denominator * 2.0;
    p.z = p.z / denominator * renderConstants.heightZoom;

    p = rotateX(p, renderConstants.rotationX);
    p = rotateZ(p, renderConstants.rotationZ);

    p *= renderConstants.zoom;
    p.x /= max(renderConstants.aspect, 0.0001);

    var normal = vertex.norm;
    normal = rotateX(normal, renderConstants.rotationX);
    normal = rotateZ(normal, renderConstants.rotationZ);

    var output : VertexOutput;

    // Z WebGPU zakres głębokości wynosi 0..1.
    output.position = vec4<f32>(
        p.x,
        -p.y,
        clamp(0.5 - p.z * 0.35, 0.0, 1.0),
        1.0
    );

    output.normal = normalize(normal);
    output.height = vertex.pos.z;

    return output;
}

@fragment
fn fsMain(input : VertexOutput) -> @location(0) vec4<f32> {
    let lightDirection = normalize(renderConstants.lightDir.xyz);

    let diffuse = max(
        dot(normalize(input.normal), -lightDirection),
        0.0
    );

    let ambient = 0.20;
    let lighting = ambient + diffuse * 0.80;

    let waveTint = vec3<f32>(
        0.05,
        0.20 + clamp(abs(input.height) * 0.01, 0.0, 0.25),
        0.65
    );

    let color = mix(
        renderConstants.baseColor.rgb,
        waveTint,
        0.40
    );

    return vec4<f32>(color * lighting, 1.0);
}
`;

class WaveWebGpu {
    constructor(canvas, options) {
        this.canvas = canvas;

        this.nx = Math.max(3, Number(options.nx ?? 100));
        this.ny = Math.max(3, Number(options.ny ?? 100));

        this.dt = Number(options.dt ?? 0.016);
        this.w = Number(options.w ?? 1.0);
        this.amplitude = Number(options.amplitude ?? 1.0);

        this.time = 0;
        this.frameNumber = 0;
        this.running = false;
        this.disposed = false;
        this.animationFrameId = 0;

        this.resizeObserver = null;
    }

    async initialize() {
        if (!navigator.gpu) {
            throw new Error(
                "Ta przeglądarka nie udostępnia WebGPU. " +
                "Uruchom aplikację przez HTTPS i użyj aktualnego Edge lub Chrome."
            );
        }

        this.adapter = await navigator.gpu.requestAdapter({
            powerPreference: "high-performance"
        });

        if (!this.adapter) {
            throw new Error("Nie znaleziono adaptera WebGPU.");
        }

        this.device = await this.adapter.requestDevice();

        this.device.lost.then(info => {
            this.running = false;

            console.error(
                "Urządzenie WebGPU zostało utracone:",
                info.message,
                info.reason
            );
        });

        this.context = this.canvas.getContext("webgpu");

        if (!this.context) {
            throw new Error(
                "Nie udało się pobrać kontekstu WebGPU dla canvas."
            );
        }

        this.canvasFormat = navigator.gpu.getPreferredCanvasFormat();

        this.context.configure({
            device: this.device,
            format: this.canvasFormat,
            alphaMode: "opaque"
        });

        this.createBuffers();
        this.createComputePipelines();
        this.createRenderPipeline();
        this.createBindGroups();
        this.installResizeObserver();

        this.updateCanvasSize();
        this.writeSimulationConstants();
        this.writeRenderConstants();
    }

    createBuffers() {
        const pointCount = this.nx * this.ny;

        // Punkt: 3 x f32 = 12 bajtów.
        const pointStride = 12;
        const pointsByteLength = pointCount * pointStride;

        // PunktNormal zawiera dwa vec3<f32>.
        // Każdy vec3 ma alignment 16, więc struktura zajmuje 32 bajty.
        const vertexStride = 32;
        const verticesByteLength = pointCount * vertexStride;

        this.pointBufferA = this.device.createBuffer({
            label: "Wave points A",
            size: pointsByteLength,
            usage:
                GPUBufferUsage.STORAGE |
                GPUBufferUsage.COPY_DST |
                GPUBufferUsage.COPY_SRC
        });

        this.pointBufferB = this.device.createBuffer({
            label: "Wave points B",
            size: pointsByteLength,
            usage:
                GPUBufferUsage.STORAGE |
                GPUBufferUsage.COPY_DST |
                GPUBufferUsage.COPY_SRC
        });

        this.vertexBuffer = this.device.createBuffer({
            label: "Wave positions and normals",
            size: verticesByteLength,
            usage:
                GPUBufferUsage.STORAGE |
                GPUBufferUsage.COPY_SRC
        });

        this.simulationUniformBuffer = this.device.createBuffer({
            label: "Simulation constants",
            size: 32,
            usage:
                GPUBufferUsage.UNIFORM |
                GPUBufferUsage.COPY_DST
        });

        this.renderUniformBuffer = this.device.createBuffer({
            label: "Render constants",
            size: 64,
            usage:
                GPUBufferUsage.UNIFORM |
                GPUBufferUsage.COPY_DST
        });

        const initialPoints = new Float32Array(pointCount * 3);

        for (let i = 0; i < pointCount; i++) {
            initialPoints[i * 3 + 0] = 1.0; // m
            initialPoints[i * 3 + 1] = 0.0; // v
            initialPoints[i * 3 + 2] = 0.0; // x
        }

        this.initialPoints = initialPoints;

        this.device.queue.writeBuffer(
            this.pointBufferA,
            0,
            initialPoints
        );

        this.device.queue.writeBuffer(
            this.pointBufferB,
            0,
            initialPoints
        );

        this.indices = this.createIndices();

        this.indexBuffer = this.device.createBuffer({
            label: "Wave index buffer",
            size: Math.max(4, this.indices.byteLength),
            usage:
                GPUBufferUsage.INDEX |
                GPUBufferUsage.COPY_DST
        });

        this.device.queue.writeBuffer(
            this.indexBuffer,
            0,
            this.indices
        );

        this.createDepthTexture();
    }

    createIndices() {
        const indexCount =
            (this.nx - 1) *
            (this.ny - 1) *
            6;

        const indices = new Uint32Array(indexCount);

        let position = 0;

        for (let i = 0; i < this.nx - 1; i++) {
            for (let j = 0; j < this.ny - 1; j++) {
                const p00 = i * this.ny + j;
                const p01 = i * this.ny + j + 1;
                const p10 = (i + 1) * this.ny + j;
                const p11 = (i + 1) * this.ny + j + 1;

                indices[position++] = p00;
                indices[position++] = p01;
                indices[position++] = p10;

                indices[position++] = p10;
                indices[position++] = p01;
                indices[position++] = p11;
            }
        }

        return indices;
    }

    createComputePipelines() {
        const simulationModule = this.device.createShaderModule({
            label: "Wave simulation shader",
            code: COMPUTE_WGSL
        });

        const normalsModule = this.device.createShaderModule({
            label: "Wave normals shader",
            code: NORMALS_WGSL
        });

        this.simulationPipeline =
            this.device.createComputePipeline({
                label: "Wave simulation pipeline",
                layout: "auto",
                compute: {
                    module: simulationModule,
                    entryPoint: "obliczWspolrzedne"
                }
            });

        this.normalsPipeline =
            this.device.createComputePipeline({
                label: "Wave normals pipeline",
                layout: "auto",
                compute: {
                    module: normalsModule,
                    entryPoint: "obliczNormalne"
                }
            });
    }

    createRenderPipeline() {
        const renderModule = this.device.createShaderModule({
            label: "Wave render shader",
            code: RENDER_WGSL
        });

        this.renderPipeline = this.device.createRenderPipeline({
            label: "Wave render pipeline",
            layout: "auto",

            vertex: {
                module: renderModule,
                entryPoint: "vsMain"
            },

            fragment: {
                module: renderModule,
                entryPoint: "fsMain",
                targets: [
                    {
                        format: this.canvasFormat
                    }
                ]
            },

            primitive: {
                topology: "triangle-list",
                frontFace: "ccw",
                cullMode: "none"
            },

            depthStencil: {
                format: "depth24plus",
                depthWriteEnabled: true,
                depthCompare: "less"
            }
        });
    }

    createBindGroups() {
        const simulationLayout =
            this.simulationPipeline.getBindGroupLayout(0);

        const normalsLayout =
            this.normalsPipeline.getBindGroupLayout(0);

        const renderLayout =
            this.renderPipeline.getBindGroupLayout(0);

        // A -> B
        this.simulationBindGroupAB =
            this.device.createBindGroup({
                label: "Simulation A to B",
                layout: simulationLayout,
                entries: [
                    {
                        binding: 0,
                        resource: {
                            buffer: this.simulationUniformBuffer
                        }
                    },
                    {
                        binding: 1,
                        resource: {
                            buffer: this.pointBufferA
                        }
                    },
                    {
                        binding: 2,
                        resource: {
                            buffer: this.pointBufferB
                        }
                    }
                ]
            });

        // B -> A
        this.simulationBindGroupBA =
            this.device.createBindGroup({
                label: "Simulation B to A",
                layout: simulationLayout,
                entries: [
                    {
                        binding: 0,
                        resource: {
                            buffer: this.simulationUniformBuffer
                        }
                    },
                    {
                        binding: 1,
                        resource: {
                            buffer: this.pointBufferB
                        }
                    },
                    {
                        binding: 2,
                        resource: {
                            buffer: this.pointBufferA
                        }
                    }
                ]
            });

        this.normalsBindGroupA =
            this.device.createBindGroup({
                label: "Normals from A",
                layout: normalsLayout,
                entries: [
                    {
                        binding: 0,
                        resource: {
                            buffer: this.simulationUniformBuffer
                        }
                    },
                    {
                        binding: 1,
                        resource: {
                            buffer: this.pointBufferA
                        }
                    },
                    {
                        binding: 2,
                        resource: {
                            buffer: this.vertexBuffer
                        }
                    }
                ]
            });

        this.normalsBindGroupB =
            this.device.createBindGroup({
                label: "Normals from B",
                layout: normalsLayout,
                entries: [
                    {
                        binding: 0,
                        resource: {
                            buffer: this.simulationUniformBuffer
                        }
                    },
                    {
                        binding: 1,
                        resource: {
                            buffer: this.pointBufferB
                        }
                    },
                    {
                        binding: 2,
                        resource: {
                            buffer: this.vertexBuffer
                        }
                    }
                ]
            });

        this.renderBindGroup = this.device.createBindGroup({
            label: "Wave render bind group",
            layout: renderLayout,
            entries: [
                {
                    binding: 0,
                    resource: {
                        buffer: this.renderUniformBuffer
                    }
                },
                {
                    binding: 1,
                    resource: {
                        buffer: this.vertexBuffer
                    }
                }
            ]
        });
    }

    writeSimulationConstants() {
        const data = new ArrayBuffer(32);

        const floats = new Float32Array(data);
        const uints = new Uint32Array(data);

        floats[0] = this.dt;
        floats[1] = this.w;

        uints[2] = this.nx;
        uints[3] = this.ny;

        floats[4] = Math.sin(this.time) * this.amplitude;
        floats[5] = this.time;
        floats[6] = 0;
        floats[7] = 0;

        this.device.queue.writeBuffer(
            this.simulationUniformBuffer,
            0,
            data
        );
    }

    writeRenderConstants() {
        const width = Math.max(1, this.canvas.width);
        const height = Math.max(1, this.canvas.height);
        const aspect = width / height;

        const data = new Float32Array(16);

        data[0] = this.nx;
        data[1] = this.ny;
        data[2] = aspect;
        data[3] = 2.0;                      // heightZoom

        data[4] = 30.0 * Math.PI / 180.0;   // rotationX
        data[5] = 45.0 * Math.PI / 180.0;   // rotationZ
        data[6] = 1.20;                     // zoom
        data[7] = 0;

        data[8] = 5.2;
        data[9] = 5.0;
        data[10] = -30.0;
        data[11] = 0;

        data[12] = 0.15;
        data[13] = 0.55;
        data[14] = 0.95;
        data[15] = 1.0;

        this.device.queue.writeBuffer(
            this.renderUniformBuffer,
            0,
            data
        );
    }

    renderFrame() {
        if (!this.running || this.disposed)
            return;

        this.updateCanvasSize();
        this.writeSimulationConstants();
        this.writeRenderConstants();

        const writeToB = (this.frameNumber % 2) === 0;

        const simulationBindGroup = writeToB
            ? this.simulationBindGroupAB
            : this.simulationBindGroupBA;

        const normalsBindGroup = writeToB
            ? this.normalsBindGroupB
            : this.normalsBindGroupA;

        const encoder = this.device.createCommandEncoder({
            label: "Wave frame encoder"
        });

        // Odpowiednik pierwszego Dispatch.
        {
            const pass = encoder.beginComputePass({
                label: "Wave simulation pass"
            });

            pass.setPipeline(this.simulationPipeline);
            pass.setBindGroup(0, simulationBindGroup);

            pass.dispatchWorkgroups(
                Math.ceil(this.nx / 8),
                Math.ceil(this.ny / 8),
                1
            );

            pass.end();
        }

        // Odpowiednik drugiego Dispatch.
        {
            const pass = encoder.beginComputePass({
                label: "Wave normals pass"
            });

            pass.setPipeline(this.normalsPipeline);
            pass.setBindGroup(0, normalsBindGroup);

            pass.dispatchWorkgroups(
                Math.ceil(this.nx / 8),
                Math.ceil(this.ny / 8),
                1
            );

            pass.end();
        }

        // Odpowiednik ClearRenderTargetView,
        // ClearDepthStencilView i DrawIndexed.
        {
            const colorView =
                this.context
                    .getCurrentTexture()
                    .createView();

            const pass = encoder.beginRenderPass({
                label: "Wave render pass",

                colorAttachments: [
                    {
                        view: colorView,
                        clearValue: {
                            r: 0.098,
                            g: 0.098,
                            b: 0.439,
                            a: 1.0
                        },
                        loadOp: "clear",
                        storeOp: "store"
                    }
                ],

                depthStencilAttachment: {
                    view: this.depthTextureView,
                    depthClearValue: 1.0,
                    depthLoadOp: "clear",
                    depthStoreOp: "store"
                }
            });

            pass.setPipeline(this.renderPipeline);
            pass.setBindGroup(0, this.renderBindGroup);
            pass.setIndexBuffer(this.indexBuffer, "uint32");

            pass.drawIndexed(
                this.indices.length,
                1,
                0,
                0,
                0
            );

            pass.end();
        }

        this.device.queue.submit([
            encoder.finish()
        ]);

        this.time += this.dt;
        this.frameNumber++;

        this.animationFrameId =
            requestAnimationFrame(() => this.renderFrame());
    }

    createDepthTexture() {
        this.depthTexture?.destroy();

        this.depthTexture = this.device.createTexture({
            label: "Wave depth texture",

            size: {
                width: Math.max(1, this.canvas.width),
                height: Math.max(1, this.canvas.height),
                depthOrArrayLayers: 1
            },

            format: "depth24plus",

            usage:
                GPUTextureUsage.RENDER_ATTACHMENT
        });

        this.depthTextureView =
            this.depthTexture.createView();
    }

    installResizeObserver() {
        this.resizeObserver = new ResizeObserver(() => {
            this.updateCanvasSize();
        });

        this.resizeObserver.observe(this.canvas);
    }

    updateCanvasSize() {
        const devicePixelRatio = window.devicePixelRatio || 1;

        const displayWidth = Math.max(
            1,
            Math.floor(
                this.canvas.clientWidth * devicePixelRatio
            )
        );

        const displayHeight = Math.max(
            1,
            Math.floor(
                this.canvas.clientHeight * devicePixelRatio
            )
        );

        if (
            this.canvas.width === displayWidth &&
            this.canvas.height === displayHeight
        ) {
            return;
        }

        this.canvas.width = displayWidth;
        this.canvas.height = displayHeight;

        this.createDepthTexture();
    }

    start() {
        if (this.disposed || this.running)
            return;

        this.running = true;

        this.animationFrameId =
            requestAnimationFrame(() => this.renderFrame());
    }

    stop() {
        this.running = false;

        if (this.animationFrameId) {
            cancelAnimationFrame(this.animationFrameId);
            this.animationFrameId = 0;
        }
    }

    reset() {
        this.time = 0;
        this.frameNumber = 0;

        this.device.queue.writeBuffer(
            this.pointBufferA,
            0,
            this.initialPoints
        );

        this.device.queue.writeBuffer(
            this.pointBufferB,
            0,
            this.initialPoints
        );
    }

    setParameters(parameters) {
        if (parameters.dt != null)
            this.dt = Number(parameters.dt);

        if (parameters.w != null)
            this.w = Number(parameters.w);

        if (parameters.amplitude != null)
            this.amplitude = Number(parameters.amplitude);
    }

    dispose() {
        if (this.disposed)
            return;

        this.stop();

        this.disposed = true;
        this.resizeObserver?.disconnect();

        this.pointBufferA?.destroy();
        this.pointBufferB?.destroy();
        this.vertexBuffer?.destroy();
        this.indexBuffer?.destroy();
        this.simulationUniformBuffer?.destroy();
        this.renderUniformBuffer?.destroy();
        this.depthTexture?.destroy();

        this.context?.unconfigure();
    }
}

export async function createWave(canvas, options) {
    const wave = new WaveWebGpu(canvas, options);
    await wave.initialize();
    return wave;
}