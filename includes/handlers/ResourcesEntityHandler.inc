<?php

class ResourcesEntityHandler extends ResourcesResourceBaseHandler {

  public $fileObject;
  public $resourceTypeInfo;

  public function __construct(stdClass $file, array $resource_type_info) {
    $this->fileObject = $file;
    $this->resourceTypeInfo = $resource_type_info;
  }

  /**
   * Returns a file object which can be used for validation.
   *
   * @return StdClass
   */
  public function getFileObject() {
    if (empty($this->fileObject->uri)) {
      // @TODO: Check if scheme is specified.
      $uri = $this->resourceTypeInfo['scheme'] . '://' . 'h/' . uniqid();
      $new_file = file_uri_to_object($uri, FALSE);
      // Merge file defaults.
      $file = (array) $this->fileObject + (array) $new_file;
      $file = (object) $file;
      $file->filename = $this->fileObject->filename;
      $this->fileObject = $file;
    }

    return $this->fileObject;
  }

  /**
   * If required, implementors can validate the embedCode.
   */
  public function validate() {
  }
}
