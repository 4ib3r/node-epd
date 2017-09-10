/**Rect object
 * @typedef {Object} Rect
 * @property {number} xMin
 * @property {number} yMin
 * @property {number} xMax
 * @property {number} yMax
 */

/**
 * FT_Face object wrapper
 * @typedef {Object} FontFace
 * @property {number} size font size
 * @property {number} height font size in px
 */

/**
 * EpdSize object
 * @typedef {Object} EpdSize
 * @property {number} width Display width in px
 * @property {number} height Display height in px
 */

 /**
  * Drawing color -1 inverse, 1 black (default), 0 white
  * @typedef {number} Color
  */

/**
 * @classdesc Main Epd driver class
 *
 * @name Epd
 * @class
 * @param {number} channel spi channel
 * @param {number} dc dc pin
 * @param {number} cs cs pin
 * @param {number} rst rst pin
 * @param {number} led additional led pin
 */

/**
 * Load ttf font
 * @method
 * @name Epd#font
 * @param {string} ttfPath font path
 * @param {number} font size
 * @returns {FontFace} Description of return value.
 */

/**
 * Return lcd resolution
 * @method
 * @name Epd#size
 * @returns {EpdSize} display width and height
 */

/**
 * Clear whole screen and set all refresh mode
 * @method
 * @name Epd#clear
 * */

/**
 * Change display mode to partial drawing
 * @method
 * @name Epd#clearPart
 * */

/**
 * Update whole screen
 * @method
 * @name Epd#update
 * */

/**
 * Update changed part of screen
 * @method
 * @name Epd#partUpdate
 * */

/**
 * Draw line
 * @method
 * @name Epd#line
 * @param {number} x start point on x axis
 * @param {number} y start point on y axis
 * @param {number} x2 end point on x axis
 * @param {number} y2 end point on y axis
 * @param {Color} col line color
 */

 /**
  * Draw rect
  * @method
  * @name Epd#rect
  * @param {number} x start point on x axis
  * @param {number} y start point on y axis
  * @param {number} width rect width
  * @param {number} height rect height
  * @param {boolean} fill If true rect is filled
  * @param {Color} col rect color
  */

  /**
     * Draw circle
     * @method
    * @name Epd#circle
    * @param {number} x mid point on x axis
    * @param {number} y mid point on y axis
    * @param {number} r circle size
    * @param {boolean} fill If true circle is filled
    * @param {Color} col circle color
   */

   /**
    * Draw pixel
    * @method
    * @name Epd#pixel
    * @param {number} x point on x axis
    * @param {number} y point on y axis
    * @param {Color} col pixel color
    */

    /**
    * Draw character from 16 bit representation
    * @method
    * @name Epd#drawChar
    * @param {number} x point on x axis
    * @param {number} y point on y axis
    * @param {number} char char code
    * @return {Rect}
     */

     /**
      * Draw string from 8bit representation
      * @method
      * @name Epd#drawString
      * @param {FontFace} font Font to draw string
      * @param {number} x x position
      * @param {number} y y position
      * @return {Rect} drawed coordinates
      */

    /**
     * Set default color to use
     * @method
     * @name Epd#color
     * @param {number} color 0 is white, 1 is black, -1 is inversion of current state
     */

var Epd = require('bindings')('epd');
module.exports = Epd;

