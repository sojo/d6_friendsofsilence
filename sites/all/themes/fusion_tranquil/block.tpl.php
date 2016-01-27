<?php
// $Id: block.tpl.php 7708 2010-07-15 18:19:58Z jeremy $
?>

<!-- start block.tpl.php -->
<div id="block-<?php print $block->module .'-'. $block->delta; ?>" class="block block-<?php print $block->module ?> <?php print $block_zebra; ?> <?php print $position; ?> <?php print $skinr; ?>">
  <div class="block-corners-top"><div class="corner-top-right corner"></div><div class="corner-top-left corner"></div></div><!-- /block-corners-top -->
  <div class="inner">
    <?php if ($edit_links): ?>
    <?php print $edit_links; ?>
    <?php endif; ?>
    <?php if ($block->subject): ?>
    <h2 class="title block-title"><?php print $block->subject ?></h2>
    <?php endif;?>
    <div class="content clearfix">
      <?php print $block->content ?>
    </div>
  </div>
  <div class="block-corners-bottom"><div class="corner-bottom-right corner"></div><div class="corner-bottom-left corner"></div></div><!-- /block-corners-bottom -->
</div>
<!-- /end block.tpl.php -->