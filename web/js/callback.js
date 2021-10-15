/**
 * Query python server for more details on node
 * @param {ForceGraph3D node} node node to get more details of 
 */
export function query_server_node_details(node) {
    $.getJSON("callback/node%id=" + node.id.toString(), function (data) {
        pids_tid_map = data;
        for (let pid in data) {
            let option_str = pid.toString();
            let pid_option = document.createElement("option");
            pid_option.setAttribute("value", option_str);
            pid_option.textContent += option_str;
            pid_select.appendChild(pid_option);
        }
    });
}