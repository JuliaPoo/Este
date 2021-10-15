class EsteGraph {
    
    /**
     * Constructs the graph
     * 
     * @param {domelement} elem Graph is plotted in this element
     * @param {string} filename Filename that contains the data to be plotted
     */
    constructor (elem, filename)
    {
        this.elem = elem;
        this.filename = filename;

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
        this._hovered_node = undefined;
        this._clicked_node = undefined;
        this.graphCallbacks = {
            nodeColor: params => node => {
                return this._clicked_node == node ? 'rgb(128,255,220)' : 
                    (this._hovered_node == node ? "rgb(255,200,128)" : "white");
            },
            onNodeHover: params => node => {
                // No change
                if (!node || this._hovered_node == node) return;
                
                // update
                this._hovered_node = node;
                this.graph.nodeColor(this.graph.nodeColor());
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
            },
            linkWidth: params => link => {
                1/params.frequencySensitivity * Math.log(link.count) + 1
            },
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

        // Create graph
        this.#createGraph();

        // Create GUI
        this.#createGUI();
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
            .jsonUrl(this.filename)
            .linkDirectionalArrowLength(10)
            .linkDirectionalArrowRelPos(0.5)
            .enableNodeDrag(false)
            .nodeColor(this.graphCallbacks.nodeColor(this.user_params))
            .onNodeHover(this.graphCallbacks.onNodeHover(this.user_params))
            .onNodeClick(this.graphCallbacks.onNodeClick(this.user_params))
            .linkWidth(this.graphCallbacks.linkWidth(this.user_params))
            .linkOpacity(this.graphCallbacks.linkOpacity(this.user_params))
            .linkCurvature(this.graphCallbacks.linkCurvature(this.user_params))
            .linkColor(this.graphCallbacks.linkColor(this.user_params))
            .linkThreeObjectExtend(true)
            .linkThreeObject(this.graphCallbacks.linkThreeObject(this.user_params))
            .linkPositionUpdate(this.graphCallbacks.linkPositionUpdate(this.user_params));
    
        this.graph.controls().dynamicDampingFactor = 0.8; // Make controls crisp
    }

    /**
     * Private method:
     * Initializes the GUI that enables
     * user to control `params`
     */
    #createGUI()
    {
        this.gui = new dat.GUI();
        this.gui.add(this.user_params, "linkOpacity", 0.0, 1.0).onChange(
            val => this.graph.linkOpacity(this.graphCallbacks.linkOpacity(this.user_params)));
        this.gui.add(this.user_params, "frequencySensitivity", 0.0, 1.0).onChange(
            val => this.graph.linkWidth(this.graphCallbacks.linkWidth(this.user_params)));
        this.gui.add(this.user_params, "fontSize", 1, 100, 1).onChange(
            val => this.graph.linkThreeObject(this.graphCallbacks.linkThreeObject(this.user_params)));
        this.gui.close(); 
    }
}