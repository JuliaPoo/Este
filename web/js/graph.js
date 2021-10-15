/** 
 * User controllable parameters 
 * that tune the look of the visualization.
 */
const params = {
    linkOpacity: 0.6,
    frequencySensitivity: 0.7,
    fontSize: 10
};

/**
 * Defining graph callbacks
 * that define the look of the visualization
 * given `params` (see above).
 */
const graphCallbacks = {
    linkWidth: params => link => 1/params.frequencySensitivity * Math.log(link.count) + 1,
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
}

/**
 * Plots the graph.
 * 
 * @param {domelement} elem Graph is plotted in this element
 * @param {string} filename Filename that contains the data to be plotted
 * 
 * @returns {ForceGraph3D} Graph object
 */
function createGraph(elem, filename)
{
    var W = elem.clientWidth;
    var H = elem.clientHeight;
    let graph = ForceGraph3D()(elem)
        .width(W)
        .height(H)
        .jsonUrl(filename)
        .linkDirectionalArrowLength(10)
        .linkDirectionalArrowRelPos(0.5)
        .enableNodeDrag(false)
        .linkWidth(graphCallbacks.linkWidth(params))
        .linkOpacity(graphCallbacks.linkOpacity(params))
        .linkCurvature(graphCallbacks.linkCurvature(params))
        .linkColor(graphCallbacks.linkColor(params))
        .linkThreeObjectExtend(true)
        .linkThreeObject(graphCallbacks.linkThreeObject(params))
        .linkPositionUpdate(graphCallbacks.linkPositionUpdate(params));

    graph.controls().dynamicDampingFactor = 0.8; // Make controls crisp

    return graph;
}

/**
 * Initializes the GUI that enables
 * user to control `params`
 * 
 * @param {ForceGraph3D} graph Graph object at which `params` is applied to
 */
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