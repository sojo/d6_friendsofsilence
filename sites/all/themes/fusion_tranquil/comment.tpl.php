<?php
// $Id: comment.tpl.php 6573 2010-02-25 01:29:06Z chris $
?>

<!-- start comment.tpl.php -->
<div class="comment <?php print $comment_classes;?> clear-block">
  <div class="content">
    <div class="comment-inner clearfix">
      <div class="comment-left">
        <?php print $picture ?>
        <?php if ($comment->new): ?>
        <a class="new"></a>
        <span id="new"><?php print $new ?></span>
        <?php endif; ?>
        <div class="submitted">
          <?php print $submitted ?>
        </div>
      </div><!-- /comment-left -->
      <div class="comment-right">
        <h3 class="title"><?php print $title ?></h3>
        <?php print $content ?>
        <?php if ($signature): ?>
        <div class="signature">
          <?php print $signature ?>
        </div>
        <?php endif; ?>
      </div><!-- /comment-right -->
    </div><!-- /comment-inner -->
    <?php if ($links): ?>
    <div class="links">
      <?php print $links ?>
    </div>
    <?php endif; ?>
  </div><!-- /content -->
</div><!-- /comment -->
<!-- /end comment.tpl.php -->