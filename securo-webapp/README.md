### Introduction

SecuRO is an E-Voting system using the Blockchain technology to provide a solution for a safer voting, as the current method can be easily leading to corruption.

### Dependencies:
* NPM
* Python 3.8 (if wanting to test on Windows)
* Grunt
* Bower

### Install Node Version Manager (nvm):

    :::sh
    wget -qO- https://raw.githubusercontent.com/creationix/nvm/v0.33.8/install.sh | bash

### Close the Terminal then open it again

### Install node.js:

    :::sh
    nvm install stable

### Install required tools:

    :::sh
    npm install -g grunt-cli
    npm install -g bower

### Install site dependencies:

    :::sh
    npm install
    bower install

### Deploy site-resources:

    :::sh
    grunt install-resources

### Deploy/run site:

    :::sh
    grunt

### Open http://localhost:3000 in your browser.
