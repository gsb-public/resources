<?php

/**
 *  @file resources/includes/ResourcesInfogramStreamWrapper.inc
 *
 *  Create a Infogram Stream Wrapper class for the Media/Resource module.
 */

/**
 *  Create an instance like this:
 *  $infogram = new ResourcesInfogramStreamWrapper('infogram://u/[userid]/id/[id]');
 */
class ResourcesInfogramStreamWrapper extends ResourcesReadOnlyStreamWrapper {

  // Overrides $base_url defined in MediaReadOnlyStreamWrapper.
  protected $base_url = '';

  /**
   * Overrides interpolateUrl() defined in ResourcesReadOnlyStreamWrapper.
   * This is an exact copy of the function in ResourcesReadOnlyStreamWrapper,
   * here in case that example is redefined or removed.
   */
  function interpolateUrl() {
    if ($parameters = $this->get_parameters()) {
      return $this->base_url . '?' . http_build_query($parameters);
    }
  }

  static function getMimeType($uri, $mapping = NULL) {
    return 'file/infogram';
  }

  function getTarget($f) {
    return FALSE;
  }

  function getOriginalThumbnailPath() {
    return '';
  }

  function getLocalThumbnailPath() {
    return '';
  }
}
