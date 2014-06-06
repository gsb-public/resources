<?php

/**
 *  @file resources/includes/ResourcesTwitterStreamWrapper.inc
 *
 *  Create a Twitter Tweet/feed Stream Wrapper class for the Resources module.
 */

/**
 *  Create an instance like this:
 *  $tweet = new ResourcesTweetStreamWrapper('tweet://u/[user-id]/s/[status-id]');
 */
class ResourcesTwitterStreamWrapper extends ResourcesReadOnlyStreamWrapper {

  // Overrides $base_url defined in ResourcesReadOnlyStreamWrapper.
  protected $base_url = 'http://twitter.com';
  protected $parameters = array('q' => NULL, 's' => NULL,  't' => NULL, 'u' => NULL, 'w' => NULL);

  /**
   * Returns a twitter url in the format.
   *
   * Overrides interpolateUrl() defined in ResourcesReadOnlyStreamWrapper.
   * This is an exact copy of the function in ResourcesReadOnlyStreamWrapper,
   * here in case that example is redefined or removed.
   */
  function interpolateUrl() {
    if ($params = $this->get_parameters()) {
      $url = $this->base_url . '/';
      if (!is_null($params['q'])) {
        $url .= 'search?q=' . $params['q'];
      }
      else {
        if (is_null($params['u'])) {
          return FALSE;
        }

        $url .= $params['u'];
        if (!is_null($params['s'])) {
          $url .= '/status/' . $params['s'];
        }
        elseif (!is_null($params['t'])) {
          $url .= '/timelines/' . $params['t'];
        }
      }

      return $url;
    }
  }

  static function getMimeType($uri, $mapping = NULL) {
    return 'application/twitter';
  }

  function getTarget($f) {
    return FALSE;
  }
}
