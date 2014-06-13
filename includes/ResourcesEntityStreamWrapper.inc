<?php

/**
 *  @file resources/includes/ResourcesEntityStreamWrapper.inc
 *
 *  Create a Entity Stream Wrapper class for the Resources module.
 */

/**
 * @class ResourcesEntityStreamWrapper
 *   Generic stream wrapper to be used for rendering files that don't have any URL that makes sense.
 *   The only parameter here is h, which is the hash that will be created on file create.
 */
class ResourcesEntityStreamWrapper extends ResourcesReadOnlyStreamWrapper {
  protected $parameters = array('h' => NULL);

  /**
   * Overrides interpolateUrl() defined in ResourcesReadOnlyStreamWrapper.
   */
  function interpolateUrl() {
    // Do nothing
  }

  static function getMimeType($uri, $mapping = NULL) {
    return 'application/entity';
  }

  function getTarget($f) {
    return FALSE;
  }
}
