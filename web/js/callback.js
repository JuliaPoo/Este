/**
 * Query python server for more details on node
 * @param {ForceGraph3D node} node node to get more details of 
 * @param {int} pid Process ID 
 * @param {int} pin_tid PIN Thread ID 
 */
export async function query_server_node_details(node, pid, pin_tid) {

    let url = "callback/node?"
        + "id=" + node.id.toString() + "&"
        + "pid=" + pid.toString() + "&"
        + "pin_tid=" + pin_tid.toString()

    const res = await fetch(url);
    const data = await res.json();
    return data;
}

/**
 * Does a get request to callback/terminate which shuts the server down
 * @returns {bool} If server successfully terminated
 */
export async function send_terminate_signal() {
    let url = "callback/terminate"
    const res = await fetch(url);
    console.log(res);
    return res.ok;
}

