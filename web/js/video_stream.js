$(() => {
    const gl = init_camera_frame();

    const web_socket = new WebSocket("ws://" + window.location.hostname + ":8085/camera");

    web_socket.onopen = () => {
        console.log("Web socket for camera stream is connected");
    }

    web_socket.onerror = (event) => {
        console.log("Web socket for camera stream encountered an error:", event);
    }

    let first_message = true;

    let out_data = {
        bytes: null,
        ptr: null,
        heap: null
    };

    let src_data = {
        bytes: 0,
        ptr: null,
        heap: null
    }

    web_socket.onmessage = async (msg) => {
        const buffer = await msg.data.arrayBuffer();
        const view = new DataView(buffer);
        const width = view.getUint32(0, true);
        const height = view.getUint32(4, true);

        if (first_message) {
            first_message = false;
            out_data.bytes = width * height * 4;
            out_data.ptr = Module._malloc(out_data.bytes);
            out_data.heap = new Uint8Array(Module.HEAPU8.buffer, out_data.ptr, out_data.bytes);
        }

        const num_bytes = buffer.byteLength - 8;
        if (num_bytes > src_data.bytes) {
            if (src_data.heap) {
                Module._free(src_data.heap.byteOffset);
            }

            const src_ptr = Module._malloc(num_bytes);
            const src_heap = new Uint8Array(Module.HEAPU8.buffer, src_ptr, num_bytes);
            src_data.bytes = num_bytes;
            src_data.ptr = src_ptr;
            src_data.heap = src_heap;
        }

        src_data.heap.set(new Uint8Array(buffer, 8));

        Module._yuv_to_rgb(src_data.heap.byteOffset, width, height, out_data.heap.byteOffset);

        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, out_data.heap);
    }
});