| image |
image := Form fromFileNamed: 'assets/images/clouds.png'.
image asMorph
    extent: 512@512;
    openInWindow.

OSWindow mainLoop.