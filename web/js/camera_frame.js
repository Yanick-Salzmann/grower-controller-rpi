function init_camera_frame() {
    const video_canvas = $('#camera-player')[0];
    const gl = video_canvas.getContext("webgl");
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.viewport(0, 0, 608, 480);
    gl.disable(gl.CULL_FACE);

    const vb = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vb);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
        -1.0, 1.0, 0.0, 0.0,
        1.0, 1.0, 1.0, 0.0,
        1.0, -1.0, 1.0, 1.0,
        -1.0, -1.0, 0.0, 1.0
    ]), gl.STATIC_DRAW);

    const vs = gl.createShader(gl.VERTEX_SHADER);
    const fs = gl.createShader(gl.FRAGMENT_SHADER);

    gl.shaderSource(vs, `
    precision highp float;
    
    attribute vec2 position0;
    attribute vec2 texCoord0;
    
    varying vec2 texCoord;
    
    void main() {
        gl_Position = vec4(position0, 0.0, 1.0);
        texCoord = texCoord0;
    }
    `);

    gl.shaderSource(fs, `
    precision highp float;
    
    varying vec2 texCoord;
    
    uniform sampler2D camera_texture;
    
    void main() {
        gl_FragColor = texture2D(camera_texture, texCoord);
    }
    `);

    gl.compileShader(vs);
    if (!gl.getShaderParameter(vs, gl.COMPILE_STATUS)) {
        console.error('An error occurred compiling the shaders: ', gl.getShaderInfoLog(vs));
        gl.deleteShader(vs);
    }

    gl.compileShader(fs);
    if (!gl.getShaderParameter(fs, gl.COMPILE_STATUS)) {
        console.error('An error occurred compiling the shaders: ', gl.getShaderInfoLog(fs));
        gl.deleteShader(fs);
    }

    const prog = gl.createProgram();
    gl.attachShader(prog, vs);
    gl.attachShader(prog, fs);
    gl.linkProgram(prog);

    if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
        console.error('Unable to initialize the shader program:', gl.getProgramInfoLog(prog));
    }

    gl.useProgram(prog);

    const attr_position = gl.getAttribLocation(prog, "position0");
    const attr_texcoord = gl.getAttribLocation(prog, "texCoord0");

    const uniform_tex = gl.getUniformLocation(prog, "camera_texture");
    gl.uniform1i(uniform_tex, 0);

    const cam_texture = gl.createTexture();

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, cam_texture);

    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 2, 2, 0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array([
        0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0x00, 0xFF, 0x3F, 0x7F, 0xFF, 0xFF
    ]));

    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);


    function on_frame() {
        gl.vertexAttribPointer(attr_position, 2, gl.FLOAT, false, 16, 0);
        gl.enableVertexAttribArray(attr_position);
        gl.vertexAttribPointer(attr_texcoord, 2, gl.FLOAT, false, 16, 8);
        gl.enableVertexAttribArray(attr_texcoord);

        gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

        requestAnimationFrame(on_frame);
    }

    on_frame();

    return gl;
}