/*
 * Copyright (C) 2015 Apple Inc. All rights reserved.
 * Copyright (C) 2013 University of Washington. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

WebInspector.ProbeSetDetailsSection = class ProbeSetDetailsSection extends WebInspector.DetailsSection
{
    constructor(probeSet)
    {
        console.assert(probeSet instanceof WebInspector.ProbeSet, "Invalid ProbeSet argument:", probeSet);

        var optionsElement = document.createElement("div");
        var dataGrid = new WebInspector.ProbeSetDataGrid(probeSet);

        var singletonRow = new WebInspector.DetailsSectionRow;
        singletonRow.element.appendChild(dataGrid.element);

        var probeSectionGroup = new WebInspector.DetailsSectionGroup([singletonRow]);

        super("probe", "", [probeSectionGroup], optionsElement);

        this.element.classList.add("probe-set");

        this._listenerSet = new WebInspector.EventListenerSet(this, "ProbeSetDetailsSection UI listeners");
        this._probeSet = probeSet;
        this._dataGrid = dataGrid;

        this._navigationBar = new WebInspector.NavigationBar;
        this._optionsElement.appendChild(this._navigationBar.element);

        this._addProbeButtonItem = new WebInspector.ButtonNavigationItem("add-probe", WebInspector.UIString("Add probe expression"), "Images/Plus13.svg", 13, 13);
        this._addProbeButtonItem.addEventListener(WebInspector.ButtonNavigationItem.Event.Clicked, this._addProbeButtonClicked, this);
        this._navigationBar.addNavigationItem(this._addProbeButtonItem);

        this._clearSamplesButtonItem = new WebInspector.ButtonNavigationItem("clear-samples", WebInspector.UIString("Clear samples"), "Images/NavigationItemTrash.svg", 14, 14);
        this._clearSamplesButtonItem.addEventListener(WebInspector.ButtonNavigationItem.Event.Clicked, this._clearSamplesButtonClicked, this);
        this._clearSamplesButtonItem.enabled = this._probeSetHasSamples();
        this._navigationBar.addNavigationItem(this._clearSamplesButtonItem);

        this._removeProbeButtonItem = new WebInspector.ButtonNavigationItem("remove-probe", WebInspector.UIString("Remove probe"), "Images/CloseLarge.svg", 12, 12);
        this._removeProbeButtonItem.addEventListener(WebInspector.ButtonNavigationItem.Event.Clicked, this._removeProbeButtonClicked, this);
        this._navigationBar.addNavigationItem(this._removeProbeButtonItem);

        this._listenerSet.register(this._probeSet, WebInspector.ProbeSet.Event.SampleAdded, this._probeSetSamplesChanged);
        this._listenerSet.register(this._probeSet, WebInspector.ProbeSet.Event.SamplesCleared, this._probeSetSamplesChanged);

        // Update the source link when the breakpoint's resolved state changes,
        // so that it can become a live location link when possible.
        this._updateLinkElement();
        this._listenerSet.register(this._probeSet.breakpoint, WebInspector.Breakpoint.Event.ResolvedStateDidChange, this._updateLinkElement);

        this._listenerSet.install();
    }

    // Public

    closed()
    {
        this._listenerSet.uninstall(true);
        this.element.remove();
    }

    // Protected

    sizeDidChange()
    {
        // FIXME: <https://webkit.org/b/152269> Web Inspector: Convert DetailsSection classes to use View
        this._dataGrid.sizeDidChange();
    }

    // Private

    _updateLinkElement()
    {
        var breakpoint = this._probeSet.breakpoint;
        if (breakpoint.sourceCodeLocation.sourceCode)
            this.titleElement = WebInspector.createSourceCodeLocationLink(breakpoint.sourceCodeLocation);
        else {
            // Fallback for when we can't create a live source link.
            console.assert(!breakpoint.resolved);

            var location = breakpoint.sourceCodeLocation;
            this.titleElement = WebInspector.linkifyLocation(breakpoint.contentIdentifier, location.displayLineNumber, location.displayColumnNumber);
        }

        this.titleElement.classList.add(WebInspector.ProbeSetDetailsSection.DontFloatLinkStyleClassName);
    }

    _addProbeButtonClicked(event)
    {
        function createProbeFromEnteredExpression(visiblePopover, event)
        {
            if (event.keyCode !== 13)
                return;
            let expression = event.target.value;
            this._probeSet.createProbe(expression);
            visiblePopover.dismiss();
        }

        let popover = new WebInspector.Popover;
        let content = document.createElement("div");
        content.classList.add(WebInspector.ProbeSetDetailsSection.ProbePopoverElementStyleClassName);
        content.createChild("div").textContent = WebInspector.UIString("Add New Probe Expression");
        let textBox = content.createChild("input");
        textBox.addEventListener("keypress", createProbeFromEnteredExpression.bind(this, popover));
        textBox.addEventListener("click", function (event) { event.target.select(); });
        textBox.type = "text";
        textBox.placeholder = WebInspector.UIString("Expression");
        popover.content = content;
        let target = WebInspector.Rect.rectFromClientRect(event.target.element.getBoundingClientRect());
        popover.present(target, [WebInspector.RectEdge.MAX_Y, WebInspector.RectEdge.MIN_Y, WebInspector.RectEdge.MAX_X]);
        popover.windowResizeHandler = () => {
            let target = WebInspector.Rect.rectFromClientRect(event.target.element.getBoundingClientRect());
            popover.present(target, [WebInspector.RectEdge.MAX_Y, WebInspector.RectEdge.MIN_Y, WebInspector.RectEdge.MAX_X]);
        };
        textBox.select();
    }

    _clearSamplesButtonClicked(event)
    {
        this._probeSet.clearSamples();
    }

    _removeProbeButtonClicked(event)
    {
        this._probeSet.clear();
    }

    _probeSetSamplesChanged(event)
    {
        this._clearSamplesButtonItem.enabled = this._probeSetHasSamples();
    }

    _probeSetHasSamples()
    {
        return this._probeSet.probes.some((probe) => probe.samples.length);
    }
};

WebInspector.ProbeSetDetailsSection.DontFloatLinkStyleClassName = "dont-float";
WebInspector.ProbeSetDetailsSection.ProbePopoverElementStyleClassName = "probe-popover";
