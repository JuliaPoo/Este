/**
 * Query python server for more details on node
 * @param {ForceGraph3D node} node node to get more details of 
 */
export function query_server_node_details(node) {
    return $.getJSON("callback/node%id=" + node.id.toString(), function (data) {
        return data;
    });
}