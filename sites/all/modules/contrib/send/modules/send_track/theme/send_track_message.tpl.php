<h2>Summary</h3>
<?php foreach ($summary as $activity => $info):?>
  <div class="send-track-summary send-track-summary-<?php print $activity?>">
  <h3><?php print $info['title']?></h3>
  <?php foreach ($info['data'] as $sub_info):?>
    <div class="send-track-summary-details-<?php print $activity?>">
      <strong><?php print $sub_info['title']?>:</strong>  
      <?php print $sub_info['value']?>
  <?php endforeach?>
  </div>
<?php endforeach?>

<?php foreach ($details as $activity => $info):?>
<h2><?php print $info['title']?></h2>
<?php print $info['value']?>
<?php endforeach?>
