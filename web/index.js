const params = {
  linkOpacity: 0.6,
  frequencySensitivity: 0.7,
  fontSize: 10
};

let hoverNode = null;
const highlightNodes = new Set();
const highlightLinks = new Set();

const graphCallbacks = {
  nodeColor: params => node => highlightNodes.has(node) ? node === hoverNode ? 'rgb(255,0,0,1)' : 'rgba(255,160,0,0.8)' : 'rgba(0,255,255,0.6)',
  onNodeHover: params => node => {
    // no state change
    if ((!node && !highlightNodes.size) || (node && hoverNode === node)) return;

    highlightNodes.clear();
    highlightLinks.clear();
    if (node) {
      highlightNodes.add(node);
      node.neighbors.forEach(neighbor => highlightNodes.add(neighbor));
      node.links.forEach(link => highlightLinks.add(link));
    }
    hoverNode = node || null;
    updateHighlight(graph);
  },
  linkWidth: params => link => highlightLinks.has(link) ? 4 : 1/params.frequencySensitivity * Math.log(link.count) + 1,
  linkOpacity: params => params.linkOpacity,
  linkCurvature: params => link => (link.target == link.source) ? 1 : 0,
  linkColor: params => link => (link.hasOwnProperty('non_white')) ? "green" : 0,
  linkThreeObject: params => link => {
      // extend link with text sprite
      const sprite = new SpriteText(link.rtn_called);
      sprite.color = 'white';
      sprite.textHeight = params.fontSize;
      return sprite;
    },
  linkPositionUpdate: params => (sprite, { start, end }) => {
      const middlePos = Object.assign(...['x', 'y', 'z'].map(c => ({
        [c]: start[c] + (end[c] - start[c]) / 2 // calc middle point
      })));
      // Position sprite
      Object.assign(sprite.position, middlePos);
    },
  linkDirectionalParticles: params => link => {
    return highlightLinks.has(link) ? 4 : 0;
  },
  linkDirectionalParticleSpeed: params => link => {
    return 1/params.frequencySensitivity * Math.log(link.count) + 1;
  },
  onLinkHover: params => link => {
    highlightNodes.clear();
    highlightLinks.clear();

    if (link) {
      highlightLinks.add(link);
      highlightNodes.add(link.source);
      highlightNodes.add(link.target);
    }

    updateHighlight(graph);
  },
}

function createGraph(elem, gData)
{
  var W = elem.clientWidth;
  var H = elem.clientHeight;
  var graph = ForceGraph3D()(elem)
    // general graph configurations
    .width(W)
    .height(H)
    .graphData(gData)

    // graph node configurations
    .nodeColor(graphCallbacks.nodeColor(params))
    .enableNodeDrag(false)
    .onNodeHover(graphCallbacks.onNodeHover(params))

    // graph link configurations
    .linkDirectionalArrowLength(10)
    .linkDirectionalArrowRelPos(0.5)
    .linkWidth(graphCallbacks.linkWidth(params))
    .linkOpacity(graphCallbacks.linkOpacity(params))
    .linkCurvature(graphCallbacks.linkCurvature(params))
    .linkColor(graphCallbacks.linkColor(params))
    .linkThreeObjectExtend(true)
    .linkThreeObject(graphCallbacks.linkThreeObject(params))
    .linkPositionUpdate(graphCallbacks.linkPositionUpdate(params))
    .linkDirectionalParticles(graphCallbacks.linkDirectionalParticles(params))
    .linkDirectionalParticleWidth(4)
    .onLinkHover(graphCallbacks.onLinkHover(params));

  graph.controls().dynamicDampingFactor = 0.8; // Make controls crisp
  return graph;
}

function createGUI(graph)
{
  const gui = new dat.GUI();
  gui.add(params, "linkOpacity", 0.0, 1.0).onChange(
    val => graph.linkOpacity(graphCallbacks.linkOpacity(params)));
  gui.add(params, "frequencySensitivity", 0.0, 1.0).onChange(
    val => graph.linkWidth(graphCallbacks.linkWidth(params)));
  gui.add(params, "fontSize", 1, 100, 1).onChange(
    val => graph.linkThreeObject(graphCallbacks.linkThreeObject(params)));
  gui.close(); 
}

let pids_tid_map;
let pid_select = document.getElementById("pid-selection");
let tid_select = document.getElementById("tid-selection");

$.getJSON( "gen/pid_tid_map.json", function( data ) {
  pids_tid_map = data;
  for (let pid in data) {
    let option_str = pid.toString();
    let pid_option = document.createElement("option");
    pid_option.setAttribute("value", option_str);
    pid_option.textContent += option_str;
    pid_select.appendChild(pid_option);
  }
})

pid_select.addEventListener("change", function() {
    let pid = pid_select.value;

    if (pid === "none") {
      tid_select.setAttribute("disabled", true);
      tid_select.value = "none"
      return;
    }

    while (tid_select.childElementCount != 1) {
      tid_select.removeChild(tid_select.lastChild);
    }
    let tids = pids_tid_map[pid];
    for (let tid of tids) {
      let option_str = tid.pin_tid.toString() + ": " + tid.os_tid.toString()
      let tid_option = document.createElement("option");
      tid_option.setAttribute("value", tid.pin_tid.toString());
      tid_option.textContent += option_str;
      tid_select.appendChild(tid_option);
    }
    tid_select.removeAttribute('disabled');
});

tid_select.addEventListener("change", function() {

  if (tid_select.value == "none") {
    return;
  }

  let pid = pid_select.value;
  let pin_tid = tid_select.value;
  let os_tid;
  for (let tid of pids_tid_map[pid]) {
    if (tid.pin_tid == pin_tid) {
      os_tid = tid.os_tid;
      break;
    }
  }
  
  let elem = document.getElementById('graph-3d');
  let filename = "gen/pid" + pid.toString() +
    "_tid" + os_tid.toString() +
    "_ptid" + pin_tid.toString() +
    ".json";

  let gData;
  $.getJSON(filename, function( data ) {
    if (data != null){
      gData = data;

      // cross-link node objects
      gData.links.forEach(link => {
        const a = gData.nodes[link.source];
        const b = gData.nodes[link.target];
        !a.neighbors && (a.neighbors = []);
        !b.neighbors && (b.neighbors = []);
        a.neighbors.push(b);
        b.neighbors.push(a);

        !a.links && (a.links = []);
        !b.links && (b.links = []);
        a.links.push(link);
        b.links.push(link);
      });

      graph = createGraph(elem, gData);
      createGUI(graph);
    }
  })

  
})

function updateHighlight(graph) {
  // trigger update of highlighted objects in scene
  graph
    .nodeColor(graph.nodeColor())
    .linkWidth(graph.linkWidth())
    .linkDirectionalParticles(graph.linkDirectionalParticles());
}