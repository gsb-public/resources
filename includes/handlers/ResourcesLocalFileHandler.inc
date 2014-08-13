<?php

class ResourcesLocalFileHandler extends ResourcesResourceBaseHandler {

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
    return $this->fileObject;
  }

  /**
   * If required, implementors can validate the embedCode.
   */
  public function validate() {
  }
}
