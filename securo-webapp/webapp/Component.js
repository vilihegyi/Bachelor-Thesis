sap.ui.define([
    "sap/ui/core/UIComponent",
    "sap/ui/core/routing/HashChanger"
], function (UIComponent, HashChanger) {
    "use strict";
    return UIComponent.extend("EVotingSystem.Component", {
        metadata: {
            manifest: "json"
        },
        init: function () {
            HashChanger.getInstance().replaceHash("");
            // call the init function of the parent
            UIComponent.prototype.init.apply(this, arguments);
            // create the views based on the url/hash
            this.getRouter().initialize(); // this opens up the details page
        },

        getContentDensityClass: function () {
            if (!this.sContentDensityClass) {
                if (!sap.ui.Device.support.touch){
                    this.sContentDensityClass = "sapUiSizeCompact";
                } else {
                    this.sContentDensityClass = "sapUiSizeCozy";
                }
            }
            return this.sContentDensityClass;
        }
    });
});
