sap.ui.define([
    "sap/m/MessageBox",
    "../controller/BaseController",
    "../lib/Blockchain"
], function (MessageBox, BaseController, Blockchain) {
    "use strict";

    return BaseController.extend("EVotingSystem.controller.Details", {

        /**
         * Initializing method to make a connection with the back-end
         * based on our local IP on port 8080 (or the port number we are
         * being listened on by the C++ end of the application)
         * @public
         */
        onInit : function () {
            this.blockchainInit();
        },

        /**
         * Method for trying to create a connection to the web socket where
         * we communicate with the back-end part of the application on our
         * local IP on port 8080. In case of failure or unexpected closure
         * from the back-end we display a message accordingly.
         * @private
         */
        blockchainInit: function() {
            // This is mainly used to determine if we should show which error
            // message should be shown.
            this.mBlockchainConnected = false;

            // Only initialize blockchain and event-handlers once.
            if (this.mBlockchain === undefined) {
                this.mBlockchainUrl = "//" + window.location.hostname + ":8080";
                this.mBlockchain = Blockchain.instance(this.mBlockchainUrl);

                this.mBlockchain.attachOpen(function() {
                    this.mBlockchainConnected = true;
                }, this);
                this.mBlockchain.attachError(function () {
                    if (!this.mBlockchainConnected) {
                        this.showError("Connection error", "Failed to connect to blockchain (" + this.mBlockchainUrl + ").", true);
                    }
                }, this);
                this.mBlockchain.attachClose(function () {
                    if (this.mBlockchainConnected) {
                        this.showError("Connection error", "Connection unexpectedly closed.", true);
                    }
                }, this);
            }
        },

        /**
         * Processing of the data and sending a request to the back part of
         * the application where it is further processed and validated.
         * If the vote is valid, we get a hash to confirm our details. In case
         * of error we show what failed on the back-end part.
         * @private
         */
        bSendVotePress: function() {
            let oInputCNP = this.getView().byId("iCNP");
            let oInputName = this.getView().byId("iName");
            let oInputAddress = this.getView().byId("iAddress");
            let oInputEAddress = this.getView().byId("iEAddress");
            let oRadioVoteOptions = this.getView().byId("rbVoteGroup");

            if (oInputCNP.getValue() && oInputName.getValue() &&
                oInputAddress.getValue() && oInputEAddress.getValue()) {
                const that = this;
                let cnp = oInputCNP.getValue();
                let name = oInputName.getValue();
                let address = oInputAddress.getValue();
                let emailAddress = oInputEAddress.getValue();
                let voteOption = oRadioVoteOptions.getSelectedButton().getText();

                if (this.validateCNP(cnp)) {
                    this.mBlockchain.request({
                        request: "blockchain.mine",
                        data: {
                            cnp: cnp,
                            name: name,
                            address: address,
                            emailAddress: emailAddress,
                            voteOption: voteOption
                        }
                    }, function(reply) {
                        MessageBox.show("The voting was a success! \nYour hash is: " + reply.result);
                    }, function(reply) {
                        that.showError("Error", reply.result, false);
                    });
                } else {
                    this.showError("Invalid CNP", "The inserted CNP isn't valid", false);
                }
            } else {
                this.showError("Incomplete data", "Please complete ALL the fields", false);
            }
        }, 

        /**
         * Method for generalizing errors with title, message and reload of the page.
         * @private
         * @param {string} title the title of the messagebox
         * @param {string} message the message for the content of the messagebox
         * @param {bool} reconnect refresh the webpage or not
         */
        showError: function(title, message, reconnect) {
            let options = {};

            options.icon = MessageBox.Icon.ERROR;
            options.title = title;

            if (reconnect) {
                options.actions = sap.m.MessageBox.Action.RETRY;
                options.onClose = function () {
                    window.location.reload();
                };
                options.initialFocus = MessageBox.Action.RETRY;
            } else {
                options.actions = sap.m.MessageBox.Action.OK;
                options.initialFocus = MessageBox.Action.OK;
            }

            MessageBox.show(message, options);
        },

        /**
         * Method for validating the CNP of the voter based on a
         * given algorithm for Romanian CNP's.
         * @private
         * @param {string} userCNP the CNP of the voter
         * @returns {bool} the validity of the CNP
         */
        // Validate some of the entered data
        validateCNP : function(userCNP) {
            let year = 0, hashResult = 0, cnp = [], hashTable = [2, 7, 9, 1, 4, 6, 3, 5, 8, 2, 7, 9];

            if (userCNP.length !== 13) {
                return false;
            }

            for (let i = 0; i < 13; i++) {
                cnp[i] = parseInt(userCNP.charAt(i), 10);
                if (isNaN(cnp[i])) {
                    return false;
                }
                if (i < 12) {
                    hashResult += cnp[i] * hashTable[i];
                }
            }

            hashResult %= 11;

            if (hashResult === 10) {
                hashResult = 1;
            }

            year = (cnp[1] * 10) + cnp[2];

            // Check the year and see if it's valid based on the first digit
            switch (cnp[0]) {
                case 1: case 2:
                    year += 1900;
                    break;
                case 3: case 4:
                    year += 1800;
                    break;
                case 5: case 6:
                    year += 2000;
                    break;
                case 7: case 8: case 9:
                    year += 2000;
                    if (year > (parseInt(new Date().getYear(), 10) - 14)) {
                        year -= 100;
                    }
                    break;
                default: 
                    return false;
            }

            if (year < 1800 || year > 2099) {
                return false;
            }

            return (cnp[12] === hashResult);
        }
    });
});