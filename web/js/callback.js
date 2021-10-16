/**
 * Query python server for more details on node
 * @param {ForceGraph3D node} node node to get more details of 
 * @param {int} pid Process ID 
 * @param {int} pin_tid PIN Thread ID 
 * @param {data => {}} callback Function to handle the json data
 */
export function query_server_node_details(node, pid, pin_tid, callback) {
    
    let url = "callback/node?" 
        + "id=" + node.id.toString() + "&"
        + "pid=" + pid.toString() + "&"
        + "pin_tid=" + pin_tid.toString()

    $.getJSON(url, callback);
}