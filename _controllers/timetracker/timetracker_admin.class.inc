<?php

/*
 * @author Carl McDade
 * @copyright Copyright (c) 2010, Carl McDade
 * @since 2010-02-19
 * @version 0.2
 * @license
 * @link 
 *
 */


class timetracker_admin{
	
	private $common;
	public  $page_title = 'Time Tracker Administration';
	public  $dbc;
	


    function __construct()
    {
        //Test the Common settings
        $this->common = Common::get_instance();
        $this->section = get_class($this);
    }

    function admin_links()
    {
        $section = get_class($this);
    	$var[ $section .'_1'] = array(
        		'text' => 'Entries', 
        		'path' => 'admin/timetracker/reports'
        		);
        
        $var[$section .'_2'] = array(
        		'text' => 'Projects', 
        		'path' => 'admin/timetracker/projects'
        		);
        
        $var[$section .'_3'] = array(
        		'text' => 'Groups', 
        		'path' => 'admin/timetracker/groups'
        		);
        

        return $var;
    }
    
    function index()
    {
    	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');
    	//
    	$variables['content'] = 'administration index';
    	return _view('default', $variables);  
    }
    
    
    function settings()
    {   	
    	
		// admin links
		$variables['links'] = _menu_links($this->admin_links(),' | ');
		
        $output = drupal_get_form('timetracker_projects_category_add_');
        $output .= drupal_get_form('timetracker_settings_form_');
    	$variables['content'] = $output;
        return _view('default', $variables);  	  	
    
    }
	
	function settings_save()
	{
		//
		variable_set('timetracker_projects', $_POST['selected']);
		drupal_set_message(t('The timetracker category is has been set to "'). variable_get('timetracker_projects', NULL) . '".');
		drupal_goto('admin/timetracker/settings');
	}
	
	function reports($extend = NULL)
    {			
		// admin links
		$variables['links'] = _menu_links($this->admin_links(),' | ');
			
    	$table_header = array(
					array('data' => 'ID', 'field' => 'id', 'sort' => 'asc'),
					array('data' => 'Task', 'field' => 'project_user_task'),
					array('data' => 'Name', 'field' => 'project_name'),
					array('data' => 'User', 'field' => 'user_id'),
					array('data' => 'Hours', 'field' => 'project_hours'),
					array('data' => 'Event', 'field' => 'event_date'),
					array('data' => 'Created', 'field' => 'created'),
					array('data' => 'Operations', '' => '', 'colspan' => 2),
					
					);
		
		$query = "SELECT id, project_user_task,project_name,user_id,project_hours,event_date,created FROM {timetracker_project_events} as tt";
		$query .= tablesort_sql($table_header);
		
		$limit = 10;
		
		//generate a paged query
		$result = pager_query($query, $limit);
		
		// Result is returned as a iterable object that returns a stdClass object on each iteration
		if($result->num_rows > 0)
		{
			// title for content
			$variables['title'] = t('Administer Reports');
			
			//set up data array
			$data = '';
			$i = 1;
			//your basic while loop to get the data
			while($record = db_fetch_object($result))
			{				  
					  $row_data[] = array(
							  'data' => array(
								  $record->id,
								  $record->project_user_task, 
								  $record->project_name,
								  $record->user_id,
								  $record->project_hours,
								  $record->event_date,
								  $record->created,
								  l(t('delete'), 'admin/' . APP_MODULE .'/entry_delete/'.$record->id, array('attributes' => array('class' => 'admin-delete-time', 'id' => 'del-' . $record->id))),
								  l(t('update'), 'admin/' . APP_MODULE .'/entry_update/'.$record->id, array('attributes' => array('class' => 'admin-update-time', 'id' => 'upd-' . $record->id))) 
					  
								  ),
							  'id' => 'row-'.$record->id,
							  );
			}
						
			// Format and print out the table.
			$output = theme('table',$table_header, $row_data);
			
			$output .= theme('pager', NULL, $limit, 0);
				
			// monthly totals for this user by project
			$variables['content'] = $output;
	
			return _view('default', $variables);
			
		}
		else
		{
			
			//
			$variables['content'] = t('No entries have been made. ');    
			return _view('default', $variables);
			
		}
		
    }
    
    /**
    * @author Carl McDade
    * @name project administration
    * Programmatically add and delete taxonomy terms 
    * separately from the default taxonomy admin 
    * so that Managers do not get full backend previleges
    *
    */
    
    function projects($extend = NULL)
    {
        // title for content
    	$variables['title'] = t('');
    	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');       
        
    	// get vocabulary as options list
    	
		$voc = variable_get('timetracker_projects', NULL);
		
		if(!$voc)
		{
			//
			$variables['content'] = t('No vocabulary has been assigned ' . l('set project category', 'admin/timetracker/settings') . t('. Then create some projects under that category.'));
       
			return _view('default', $variables);
		}
		
		$tree = taxonomy_get_tree($voc,0,NULL,true);
		
		$table_header = array(
				array('data' => 'ID', 'field' => 'tid', 'sort' => 'asc'),
				array('data' => 'Group', 'field' => 'project_group_id'),
				array('data' => 'Name', 'field' => 'project_name'),
				array('data' => 'Code', 'field' => 'project_external_key'),
				array('data' => 'Description', 'field' => 'description'),
				array('data' => 'Hours', 'field' => 'project_hours'),
				array('data' => 'Created', 'field' => 'created'),
				array('data' => 'Status', 'field' => 'status_name'),
				array('data' => 'Operations', '' => '', 'colspan' => 2),
				
				);
		
		$query = "SELECT td.tid, td.name, td.description, tt.project_external_key, tt.created, tt.project_hours, g.project_group_name, g.project_group_prefix, s.* FROM 
		{term_data} AS td 
		LEFT JOIN {timetracker_projects} AS tt ON td.tid = tt.project_id 
		LEFT JOIN {timetracker_project_groups} AS g ON g.project_group_id = tt.project_group_id 
		LEFT JOIN {timetracker_project_status} AS s ON tt.status = s.id WHERE td.vid =" . $voc;
		
		$query .= tablesort_sql($table_header);
		
		$limit = 10;
		
		//generate a paged query
		$result = pager_query($query, $limit);
			
		// Result is returned as a iterable object that returns a stdClass object on each iteration
		if($result->num_rows > 0){
			//set up data array
			$data = '';
			$i = 1;
			//your basic while loop to get the data
			while($record = db_fetch_object($result))
			{				
			
				/* get the total number of hours for each project */
				$hours_total = 0;
				
				$hours = db_query("SELECT e.project_hours FROM {timetracker_project_events} AS e WHERE project_id =%d", array($record->tid));
					
						
				while($event = db_fetch_object($hours))
				{
					$hours_total += $event->project_hours;
				}
				
				//$output .= print_r($record,1);
				$row_data[] = array(
						'data' => array(
								$record->tid,
								$record->project_group_name, 
								$record->name,
								$record->project_group_prefix . '-' . $record->project_external_key,
								$record->description,
								$hours_total,
								$record->created,
								$record->status_name,
								l(t('edit'), 'admin/'. APP_MODULE .'/project_edit/'.$record->tid, array('attributes' => array('class' => 'update-project', 'id' => 'upd-' . $record->tid))) . ' | ' .
								l(t('report'), 'admin/'. APP_MODULE .'/project_view/'.$record->tid, array('attributes' => array('class' => 'view-project', 'id' => 'view-' . $record->tid))) 
								
								),
						'id' => 'row-'.$record->tid,
						);
			
			}
			
				// show projects add form 
				$output = drupal_get_form('timetracker_projects_add_', 'admin/timetracker/project_save');
				
				$output .= '<div id="project-list">' . t('A list of available projects and their descriptions:') . '</div>';
				
				
				// Format and print out the table.
				$output .= theme('table',$table_header, $row_data);
				
				$output .= theme('pager', NULL, $limit, 0);
		}
		else
		{
			//
			$output = drupal_get_form('timetracker_projects_add_');
			$output .= '<div id="project-list">' . t('No projects have been added.') . '</div>';
				
		}
				   
    	$variables['content'] = $output;
       
        return _view('default', $variables);
    }
    
    function project_unique_key_autocomplete($string = '') 
	{
	    $pkid = $_POST['pkid'];
		$part = explode(',', $string);
		$matches = array();
	  
		$result = db_query("SELECT * FROM {timetracker_projects} WHERE project_external_key = '%s'", array(strtoupper($pkid)));
		$check_affected = db_affected_rows();
		
		if($check_affected > 0)
		{
		   $ok = 1;
		}
		else
		{
			$ok = 0;
		}		
		
		drupal_json($check_affected);
		
		exit();
	}
    
    function project_edit()
    {
    	// title for content
    	$variables['title'] = t('');

    	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');
        
    	$output = drupal_get_form('timetracker_projects_edit_');
    	$variables['content'] = $output;
        
        return _view('default', $variables);
    }
    
    function project_save($tid=NULL)
    {
    	/* we may need to know who by and when a project was created */
    	global $user;
    	
    	$voc = variable_get('timetracker_projects', NULL);
    	
    	$pr_id = check_plain($_POST['project_id']);
    	$pr_name = check_plain($_POST['project_name']);
    	$pr_desc = check_plain($_POST['project_desc']);
    	$pr_group = check_plain($_POST['project_group']);
    	$pr_key = check_plain($_POST['project_external_key']);
    	$pr_status = check_plain($_POST['project_status']);

    	$term = array();
		$term['name'] = $pr_name;
		$term['description'] = $pr_desc;
		$term['vid'] = $voc;
		
		/* save to the Drupal taxonomy system which creates a term/project */
		$new_term = taxonomy_save_term($term);		
		
		drupal_set_message( t('project taxonomy created') ); 
		
		/* get the term and information that was inserted into the database */
		$new = $term['tid'];
    			
    	$term_info = taxonomy_get_term($new);
    	
    	db_query("INSERT INTO {timetracker_projects} 
    		(project_id,project_name,project_desc,project_group_id,project_external_key,created,changed,status, vid) 
				VALUES(" . 
				"'" . $new ."',".
				"'" . $pr_name ."',". 
				"'" . $pr_desc . "'," . 
				"'" . strtoupper($pr_group) . "'," .  
				"'" . strtoupper($pr_key) . "'," . 
				"'" . date("Y-m-d H:i:s") . "'," .
				"'" . date("Y-m-d H:i:s") . "'," .
				"'" . $pr_status . "'," .
				"'" . $voc ."')" );
				
		drupal_set_message( t('project added to timetracker') );
		drupal_goto('admin/timetracker/projects');
    }
    
    function project_update()
    {
    	/* we may need to know who by and when a project was created */
    	global $user;
    	
    	$voc = variable_get('timetracker_projects', NULL);
    	$term_info = taxonomy_get_term($new);
    	
    	$pr_id = check_plain($_POST['project_id']);
    	$pr_name = check_plain($_POST['project_name']);
    	$pr_desc = check_plain($_POST['project_desc']);
    	$pr_group = check_plain($_POST['project_group']);
    	$pr_key = $_POST['project_external_key'];
    	$pr_status = check_plain($_POST['project_status']);

		// check to see if project exists then update in both the tables				
		$project_id = db_query("SELECT * FROM {term_data} AS tp WHERE tid=%d", array($pr_id));						
		
		// Since we are updating make sure the term actually exists in both tables
		if($project_id != FALSE)
		{
			$project_term = db_query("UPDATE {term_data} 
				SET name ='" . $pr_name . "'," . 
					"description ='" . $pr_desc  . "'" . 
					" WHERE tid= %d", array($pr_id));
			
			$project_term_check = db_affected_rows();
			
			// update the timetracker project table
			$project_info = db_query("UPDATE {timetracker_projects} 
				SET
					project_name ='" . $pr_name . "'," . 
					"project_desc ='" . $pr_desc  . "'," . 
					"project_group_id ='" . strtoupper($pr_group) . "'," . 
					"project_external_key ='" . strtoupper($pr_key) . "',"  .
					"changed ='" . date("Y-m-d H:i:s") . "'," . 
					"status ='" . $pr_status . "'" .
					" WHERE project_id= %d", array($pr_id));
			
			$project_info_check = db_affected_rows();
			
			// if the project information does not synch or exist then create it
			if($project_info_check == FALSE)
			{
				db_query("INSERT INTO {timetracker_projects} 
				(project_id,project_name,project_desc,project_group_id,project_external_key,created,changed,status, vid) 
					VALUES(" . 
					"'" . $pr_id ."',".
					"'" . $pr_name ."',". 
					"'" . $pr_desc . "'," . 
					"'" . strtoupper($pr_group) . "'," .  
					"'" . strtoupper($pr_key) . "'," . 
					"'" . date("Y-m-d H:i:s") . "'," .
					"'" . date("Y-m-d H:i:s") . "'," .
					"'" . $pr_status . "'," .
					"'" . $voc . "')");
			
			}		
						
			drupal_set_message(t('project updated'));
			drupal_goto('admin/timetracker/project_edit/'. $pr_id);
		}
		else
		{
			drupal_set_message(t('That project did not exist. You can create it here.'));
			drupal_goto('admin/timetracker/projects');
		}
		
    }
    
    function project_category_save()
    {
    	//    	
		$project = check_plain($_POST['project_category_name']);
		$description = check_plain($_POST['project_category_description']); 
		$machine_name = check_plain($_POST['project_category_machine_name']);
    	
    	$vocabulary = array(
				'name' => $project, //Human readable name of the vocabulary
				'module' => $machine_name,
				'description' => $description,
				'multiple' => 0, //set 1 to allow multiple selection
				'required' => 0, //set 1 to make the terms mandatory to be selected
				'hierarchy' => 0, //set 1 to allow and create hierarchy of the terms within the vocabulary
				'relations' => 0, //set 1 to set and allow relation amongst multiple terms
				'weight' => -9, //set the weight to display the vocabulary in the list
				);

		$voc = taxonomy_save_vocabulary($vocabulary); 
		
		variable_set('timetracker_projects', $voc , NULL);	
				
		drupal_set_message(t('Project category added and set  to timetracker list'));
		
		drupal_goto('admin/timetracker/settings');
		
    }
    
    function groups($extend = NULL)
    {
        // title for content
    	$variables['title'] = t('');
  	
    	// admin links
        $variables['links'] = _menu_links($this->admin_links(),' | ');
                
    	// get vocabulary as options list
		$voc = variable_get('timetracker_projects', NULL);
		$tree = taxonomy_get_tree($voc,0,NULL,true);
		
		$table_header = array(
				array('data' => 'ID', 'field' => 'project_group_id', 'sort' => 'asc'),
				array('data' => 'Name', 'field' => 'project_group_name'),
				array('data' => 'Prefix Code', 'field' => 'project_group_prefix'),
				array('data' => 'Description', 'field' => 'project_group_description'),
				'Operations',
				
				);
		
		$query = "SELECT gr.project_group_id, gr.project_group_name, gr.project_group_description, gr.project_group_prefix FROM 
		{timetracker_project_groups} AS gr";
		
		$query .= tablesort_sql($table_header);
		
		$limit = 10;
		
		//generate a paged query
		$result = pager_query($query, $limit);	
			
		// Result is returned as a iterable object that returns a stdClass object on each iteration
		if($result->num_rows > 0){
			//set up data array
			$data = '';
			$i = 1;
			//your basic while loop to get the data
			while($record = db_fetch_object($result))
			{				
			
				//$output .= print_r($record,1);
					$row_data[] = array(
						'data' => array(
							$record->project_group_id,
							$record->project_group_name, 
							$record->project_group_prefix,
							$record->project_group_description,
							l(t('edit'), 'admin/'. APP_MODULE .'/group_edit/'.$record->project_group_id, array('attributes' => array('class' => 'update-group', 'id' => 'upd-' . $record->project_group_id))) 
							),
						'id' => 'row-'.$record->project_group_id,
					);
			
			}
			// show projects add form 
			$output = drupal_get_form('timetracker_groups_add_');
			
			$output .= '<div id="project-list">' . t('A list of available groups and their descriptions:') . '</div>';
			
			
			// Format and print out the table.
			$output .= theme('table',$table_header, $row_data);
			
			$output .= theme('pager', NULL, $limit, 0);
				   
			$variables['content'] = $output;
		   
			return _view('default', $variables);
		}
		else
		{
			
			//
			$output = drupal_get_form('timetracker_groups_add_');
			$output .= t('No group entries have been made. ');
			$variables['content'] =  $output;
			
			return _view('default', $variables);
			
		}
		
		
    }
    
    function group_save()
    {
    	global $user;
    	
    	//    	
    	$group_name = $_POST['group_name'];
    	$group_prefix = $_POST['group_prefix'];
    	$group_description = $_POST['group_description'];
    	
		$group_set = db_query("INSERT INTO {timetracker_project_groups} 
			(project_group_name,project_group_prefix,project_group_description)
				 VALUES('" . $group_name ."','". 
				$group_prefix . "','" . 
				$group_description . "')");
		
		$group_set_info = db_affected_rows();
		
		if($group_set_info > 0)
		{					
			drupal_set_message( t('The group.') . $group_name . t('has been created.'));										 
		}
		else
		{
			drupal_set_message(t('There was an error in the creation of the record please contact support.'));
		}
		
		drupal_goto('admin/timetracker/groups');
		
    }
    
    function group_update()
    {
    	global $user;
    	
    	// 
    	$group_id = $_POST['group_id'];
    	$group_name = $_POST['group_name'];
    	$group_prefix = $_POST['group_prefix'];
    	$group_description = $_POST['group_description'];
    	
		$group_set = db_query("UPDATE {timetracker_project_groups} 
				 SET " .
				"project_group_name = '" . $group_name ."',". 
				"project_group_prefix = '" . $group_prefix . "'," . 
				"project_group_description = '" .  $group_description . "' WHERE project_group_id=%d", array($group_id));
		
		$group_set_info = db_affected_rows();
		
		if($group_set_info > 0)
		{					
			drupal_set_message( t('The group.') . $group_name . t('has been updated.'));										 
		}
		else
		{
			drupal_set_message(t('There was an error in the creation of the record please contact support.'));
		}
		
		drupal_goto('admin/timetracker/groups');
		
    }
    
    function group_edit()
    {
    	//
    	global $user;
    	
		$output .= drupal_get_form('timetracker_groups_add_', 'admin/timetracker/group_save');
		    	
    	//group prefix
    	
    	//group description
    	
    	// group update
    	
    	return $output;
    	
    }
    
    
}

?>
