import * as CALLBACK from "./callback.js"

export class EsteGraph {
    
    /**
     * Constructs the graph
     * 
     * @param {domelement} elem Graph is plotted in this element
     * @param {domelement} more_details More details abt a node is displayed here
     * @param {int} pid Process ID to be plotted
     * @param {int} os_tid OS Thread ID to be plotted
     * @param {int} pin_tid PIN Thread ID to be plotted
     */
    constructor (elem, more_details, pid, os_tid, pin_tid)
    {

        this.elem = elem;
        this.more_details = more_details;
        this.pid = pid;
        this.os_tid = os_tid;
        this.pin_tid = pin_tid;
        this.filename = "gen/pid" + pid.toString() +
            "_tid" + os_tid.toString() +
            "_ptid" + pin_tid.toString() +
            ".json";

        this.gData = this.#parseFile(this.filename);
        this.#crossLinkNode();
        console.log(this.gData);

        /*
        * User controllable parameters 
        * that tune the look of the visualization.
        */
        this.user_params = {
            linkOpacity: 0.6,
            frequencySensitivity: 0.7,
            fontSize: 10
        };

        /*
         * Defining graph callbacks
         * that define the look of the visualization
         * given `params` (see above).
         */
        this._highlight_nodes = new Set();
        this._highlight_links = new Set();
        this._hovered_node = undefined;
        this._clicked_node = undefined;
        this.graphCallbacks = {
            nodeColor: params => node => {
                return this._clicked_node == node ? 'rgb(128,255,220)' : 
                    (this._highlight_nodes.has(node) ? node === this._hovered_node ? 'rgb(255,0,0,1)' : "rgb(255,200,128)" : 'white');
                    // (this._hovered_node == node ? "rgb(255,200,128)" : "white");
            },
            onNodeHover: params => node => {
                // no state change
                if ((!node && !this._highlight_nodes.size) || (node && this._hover_node === node)) return;

                this._highlight_nodes.clear();
                this._highlight_links.clear();

                if (node) {
                  this._highlight_nodes.add(node);
                  node.neighbors.forEach(neighbor => this._highlight_nodes.add(neighbor));
                  node.links.forEach(link => this._highlight_links.add(link));
                }
                this._hovered_node = node || null;
                this.#updateHighlight();
            },
            onNodeClick: params => node => {
                // No change
                if (!node || this._clicked_node == node) return;
                
                // update
                if (this._clicked_node != undefined) {
                    this._clicked_node.__threeObj.scale.set(1,1,1);
                }
                this._clicked_node = node;
                this.graph.nodeColor(this.graph.nodeColor());
                this.graph.nodeRelSize(this.graph.nodeRelSize());
                node.__threeObj.scale.set(2,2,2);

                this.#displayMoreDetails(node);
            },
            linkWidth: params => link => {
                return this._highlight_links.has(link) ? 4 : 1/params.frequencySensitivity * Math.log(link.count) + 1
            },
            linkOpacity: params => params.linkOpacity,
            linkCurvature: params => link => {
                return (
                    link.target == link.source
                ) ? 1 : 0;
            },
            linkColor: params => link => {
                return (link.hasOwnProperty('non_white')) ? "green" : 0;
            },
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
            onLinkHover: params => link => {
                this._highlight_nodes.clear();
                this._highlight_links.clear();

                if (link) {
                  this._highlight_links.add(link);
                  this._highlight_nodes.add(link.source);
                  this._highlight_nodes.add(link.target);
                }

                this.#updateHighlight();
            },
            linkDirectionalParticles: params => link => {
                return this._highlight_links.has(link) ? 4 : 0;
            },
            linkDirectionalParticleSpeed: params => link => {
                return 1/params.frequencySensitivity * Math.log(link.count) + 1;
            },
        }

        // Create graph
        this.#createGraph();

        // Create GUI
        this.#createGUI();

        // Register event handlers
        this.#registerHandlers();
    }

    /**
     * Private method:
     * Parses json file contents into a obj
     * 
     */
    #parseFile(filename)
    {
        var Httpreq = new XMLHttpRequest(); // a new request
        Httpreq.open("GET",filename,false);
        Httpreq.send(null);
        return JSON.parse(Httpreq.responseText);          
    }

    /**
     * Private method:
     * Adds neighbouring nodes and links to each object in gData
     * 
     */
    #crossLinkNode()
    {
        this.gData.links.forEach(link => {
            const a = this.gData.nodes[link.source];
            const b = this.gData.nodes[link.target];
            !a.neighbors && (a.neighbors = []);
            !b.neighbors && (b.neighbors = []);
            a.neighbors.push(b);
            b.neighbors.push(a);

            !a.links && (a.links = []);
            !b.links && (b.links = []);
            a.links.push(link);
            b.links.push(link);
        });         
    }

    /**
     * Private method:
     * Trigger update of highlighted objects in scene
     * 
     */
    #updateHighlight() {
        this.graph
        .nodeColor(this.graph.nodeColor())
        .linkWidth(this.graph.linkWidth())
        .linkDirectionalParticles(this.graph.linkDirectionalParticles());
    }

    /**
     * Private method:
     * Initializes the GUI that enables
     * user to control `params`
     */
    #createGUI()
    {
        this.gui = new dat.GUI();

        this.gui.domElement.id = 'graph-config';

        this.gui.add(this.user_params, "linkOpacity", 0.0, 1.0).onChange(
            val => this.graph.linkOpacity(this.graphCallbacks.linkOpacity(this.user_params)));
        this.gui.add(this.user_params, "frequencySensitivity", 0.0, 1.0).onChange(
            val => this.graph.linkWidth(this.graphCallbacks.linkWidth(this.user_params)));
        this.gui.add(this.user_params, "fontSize", 1, 100, 1).onChange(
            val => this.graph.linkThreeObject(this.graphCallbacks.linkThreeObject(this.user_params)));

        this.gui.close(); 
    }

    /**
     * Private method:
     * Initializes the graph
     */
    #createGraph()
    {
        var W = this.elem.clientWidth;
        var H = this.elem.clientHeight;
        this.graph = ForceGraph3D()(this.elem)
            .width(W)
            .height(H)
            .graphData(this.gData)
            .linkDirectionalArrowLength(10)
            .linkDirectionalArrowRelPos(0.5)
            .nodeColor(this.graphCallbacks.nodeColor(this.user_params))
            .onNodeHover(this.graphCallbacks.onNodeHover(this.user_params))
            .onNodeClick(this.graphCallbacks.onNodeClick(this.user_params))
            .linkWidth(this.graphCallbacks.linkWidth(this.user_params))
            .linkOpacity(this.graphCallbacks.linkOpacity(this.user_params))
            .linkCurvature(this.graphCallbacks.linkCurvature(this.user_params))
            .linkColor(this.graphCallbacks.linkColor(this.user_params))
            .linkThreeObjectExtend(true)
            .linkThreeObject(this.graphCallbacks.linkThreeObject(this.user_params))
            .linkPositionUpdate(this.graphCallbacks.linkPositionUpdate(this.user_params))
            .linkDirectionalParticles(this.graphCallbacks.linkDirectionalParticles(this.user_params))
            .linkDirectionalParticleWidth(4)
            .onLinkHover(this.graphCallbacks.onLinkHover(this.user_params));
    
        this.graph.controls().dynamicDampingFactor = 0.8; // Make controls crisp
        this.elem.firstChild.style.position = "absolute";

        // this.graph.d3AlphaDecay(0);
        // this.graph.d3VelocityDecay(0.1);
    }

    /**
     * Private method:
     * Register window handles for EsteGraph
     */
    #registerHandlers()
    {
        window.addEventListener('resize', () => {

            var W = this.elem.clientWidth;
            var H = this.elem.clientHeight;

            this.graph
                .width(W)
                .height(H);

        }, false);
    }

    /**
     * Private method:
     * Display more details about the node
     * @param {ForceGraph3D node} node node to get more details of 
     */
    #displayMoreDetails(node)
    {
        CALLBACK.query_server_node_details(node, this.pid, this.pin_tid).then(node => {
            
            let dets = "Node #" + node.id.toString() + "\n\n";
            dets += "Image: " + node.image_path.toString() + "\n\n";
            dets += "Executed Count: " + node.count.toString() + "\n\n";

            dets += "Disassembly:\n";
            for (let l of node.disassembly) {
                dets += l + "\n";
            }
            dets += "\n";

            this.more_details.innerText = dets;
        });
    }
}

